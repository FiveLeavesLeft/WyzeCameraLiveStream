#include "common.h"

// wget -O - 192.168.0.124:12345/ | ffmpeg -f h264 -i - -frames:v 1 output.png

#define SA struct sockaddr 

#define MY_PORT 12345

static int server_fd = 0;
static int client_fd = 0;
static int client_channel = 0;
static int server_stop = 0;
static int client_stop = 0;
static pthread_t thread;

static int fd_has_data(int fd) {
	fd_set readfds;
	struct timeval timeout;
	timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    return select(fd+1, &readfds, 0, 0, &timeout);
}

static int client_readln(int fd, char *buf, int n) {
	char *p = buf, *end = p + n;
	while(p < end && client_stop == 0) {
		int ready = fd_has_data(fd);
		if(ready == 0) {
			continue;
		}
		if(ready < 0) {
			print("select failed, stopping client");
			client_stop = 1;
			break;
		}
		char c;
		ssize_t rval = read(fd, &c, 1);
		if(rval < 0) {
			print("read failed, stopping client");
			client_stop = 1;
			break;
		} 
		if(rval > 0) {
		    if(*p == '\r') {
				return p - buf;
		    } else if(*p != '\n') {
				*p++ = c;
		    }
		}
	}
	return p - buf;
}

static int client_send(int fd, char *buf, int n) {
	char *p = buf, *end = p + n;
	while(p < end) {
		ssize_t rval = write(fd, p, end - p);
		if(rval > 0) {
			p += rval;
		}
	}
	return 0;
}
static int client_puts(int fd, char *s) {
	return client_send(fd, s, strlen(s));
}

static int do_client(int fd, char *host) {
	char buf[64];
	client_stop = 0;
	while(client_stop == 0) {
		bzero(buf, sizeof(buf));
		int n = client_readln(fd, buf, sizeof(buf)-1);
		if(client_stop) {
				break;
		}
		print("Client said: '%s'", buf);
		if(strncmp(buf, "GET", 3) == 0) {
			if(strstr(buf, " / ")) {
				if(client_fd == 0) {
					client_puts(fd, "HTTP/1.1 200 OK\r\n");
					client_puts(fd, "\r\n");
				}
				client_fd = fd;
			}
		}
	}
	int tmp = client_fd;
	if(tmp && tmp != fd) {
		client_fd = 0;
		close(tmp);
	}
	return 0;
}

static void *server_thread(void *arg) {
	struct sockaddr_in cli;
	socklen_t slen = (socklen_t)sizeof(cli);

	server_stop = 0;
	print("Waiting...");
	while(server_stop == 0) {
		int ready = fd_has_data(server_fd);
		if(ready == 0) {
				continue;
		}
		if(ready < 0) {
			print("select failed");
			continue; // break, stop?
		}
		if(server_stop) {
				break;
		}
        print("Accept...");
        int connfd = accept(server_fd, (SA*)&cli, &slen);
        if (connfd < 0) {
            print("server acccept failed...");
			continue;
		}
		char *host = inet_ntoa(cli.sin_addr);
	    print("New client: %s", host);
		do_client(connfd, host);
		print("New client finished.");
		if(connfd == client_fd) {
			print("connfd == client_fd");
			client_fd = 0;
		}
		close(connfd);
        print("Waiting...");
	}
	return 0;
}

static int start_server() {
	if(server_fd) {
		return server_fd;
	}
    int sockfd, len;
    struct sockaddr_in servaddr;

    int port = MY_PORT;
    char *env = getenv("STREAM_HACK_PORT");
    if(env && *env) {
		if(sscanf(env, "%d", &port) != 1) {
			print("sscanf failed on %s", env);
		}
	}
	print("Start server on port %d", port);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        print("socket creation failed...");
		return -1;
	}

    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
            &enable, sizeof(int)) < 0) {
		print("setsockopt(SO_REUSEADDR) failed");
		close(sockfd);
		return -1;
	}
	bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        print("socket bind failed...");
		close(sockfd);
		return -1;
    }

    if ((listen(sockfd, 5)) != 0) {
        print("Listen failed...");
	    close(sockfd);
	    return -1;
	}
	server_fd = sockfd;
    int err = pthread_create(&thread, 0, server_thread, (void *)0);
    print("Server started");
	return 0;
}

/*
static void thread_hack() {
	static int n = 0;
	static void *threads[4];
	pthread_t thread;
	thread = pthread_self();
	void *p = (void *)&thread;
	for(int i = 0; i < 4; i++) {
		if(threads[i] == p) {
			return;
		}
	}
	for(int i = 0; i < 4; i++) {
		if(threads[i] == 0) {
			threads[i] = p;
			print("thread %p", p);
			return;
		}
	}
	if(n++ < 10) {
		print("More than 4 threads");
    }
}
*/

public int crb_tcp_hook(int rval, int chan, void *stream) {
	start_server();
	if(client_fd && chan == client_channel && rval == 0) {
		int err = crb_write_h264(client_fd, 0, stream);
		//thread_hack();
		if(err) {
			client_stop = 1;
			client_fd = 0;
			print("stream write error:%s", strerror(errno));
			print("shut down client");
			return -1;
		}
	}
	return 0;
}
