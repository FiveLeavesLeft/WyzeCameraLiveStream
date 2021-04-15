#include "common.h"

//#include "json-c/json.h"

#define SERVER_ADDR	"165.232.153.160"
#define SERVER_PORT	5005
#define MY_PORT		5005

#define PING_PERIOD	10
#define DEAD_TIMEOUT  10

#define CMD_FMT "{\"cmd\":\"%s\",\"mac\":\"%s\",\"pri\":\"%s\",\"pub\":\"%s\"}"

#define HOLE  1
#define PING  2
#define PUNCH 3
#define PONG  4
#define PLAY  5

typedef struct {
    struct sockaddr_in pri; // private address
    struct sockaddr_in pub; // public address
    struct sockaddr_in frm; // who sent this request
    int cmd;
} req_t;

typedef struct {
    char *name;
    int cmd;
} cmd_t;

typedef struct {
    unsigned long addr;
    time_t when;
} alive_t;

static int client_fd = 0;

static char my_pri[24]; // private address
static char my_pub[24]; // public address
static char my_mac[16]; // mac address

static struct sockaddr_in *play = 0;


static cmd_t cmds[] = {
   "hole",  HOLE,
   "ping",  PING,
   "pong",  PONG,
   "play",  PLAY,
   "punch", PUNCH
};
static int ncmd = sizeof(cmds)/sizeof(cmds[0]);

static alive_t alive[8];
static int nalive = sizeof(alive)/sizeof(alive[0]);

static int _snprintf(char *buf, int nbuf, char *fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        int n = vsnprintf(buf, nbuf, fmt, ap);
        va_end(ap);
        //print("sprintf %d of %d", n, nbuf);
        if(n >= nbuf) {
                print("ERROR: sprint truncate");
                return -1;
        }
        return 0;
}
static int _strncpy(char *dst, const char *src, int ndst) {
        // xxx.xxx.xxx.xxx\0
        //char *end = stpncpy(dst, src, ndst);
        char *end = strncpy(dst, src, ndst);
	if(dst[ndst-1] != 0) {
                print("src=%s len=%lu", src, strlen(src));
                print("ERROR: strcpy truncate");
                return -1;
 	}
	/*
        print("strcpy %ld of %d", end - dst, ndst);
        if(end >= dst + ndst) {
                print("src=%s len=%lu", src, strlen(src));
                print("ERROR: strcpy truncate");
                return -1;
        }
	*/
        return 0;
}

static int get_ip_and_mac(char *ip, char *mac, int n) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    bzero(&ifr, sizeof(ifr));

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want an IP address attached to "wlan0" */
    strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);
    strncpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), n);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    *mac = 0;
    for(int s = 0; s < 6; s++ ) {
            char tmp[8];
            sprintf(tmp, "%.2X", (unsigned char)ifr.ifr_hwaddr.sa_data[s]);
            strcat(mac, tmp);
    }
    close(fd);
    return 0;
}

static int can_read(int fd) {
    fd_set readfds;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    return select(fd+1, &readfds, 0, 0, &timeout);
}

static time_t time_now() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec;
}

static int is_alive(struct sockaddr_in sockaddr) {
   unsigned long addr = sockaddr.sin_addr.s_addr;
   alive_t *end = alive + nalive;
   time_t now = time_now();
   for(alive_t *p = alive; p < end; p++) {
        if(p->addr == addr && now - p->when < DEAD_TIMEOUT) {
            return 1;
        }
   }
   return 0;
}

static void mark_alive(struct sockaddr_in sockaddr) {
   unsigned long addr = sockaddr.sin_addr.s_addr;
   alive_t *end = alive + nalive, *dead = 0, *min = 0;
   time_t now = time_now();
   for(alive_t *p = alive; p < end; p++) {
        if(p->addr == addr) {
            p->when = now;
            return;
        } else if(now - p->when > DEAD_TIMEOUT) {
            p->addr = 0;
            dead = p;
        } else if(min == 0 || p->when < min->when) {
            min = p;
        }
   }
   if(dead) {
        dead->addr = addr;
        dead->when = now;
   } else {
        min->addr = addr;
        min->when = now;
   }
}

static int fill_addr(struct sockaddr_in *addr, char *host, int port) {
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = (host && *host) ?
			inet_addr(host) : htonl(INADDR_ANY);
    return 0;
}

static char *addr_ip(struct sockaddr_in *addr) {
    return inet_ntoa(addr->sin_addr);
}

static int addr_port(struct sockaddr_in *addr) {
    return (int)ntohs(addr->sin_port);
}

static int unpack_addr(struct sockaddr_in *addr, char **name, int *port) {
    *name = inet_ntoa(addr->sin_addr);
    *port = (int)ntohs(addr->sin_port);
    return 0;
}

static int parse_addr(const char *s, struct sockaddr_in *addr) {
    char host[24];
    _strncpy(host, s, sizeof(host));
    char *sep = strstr(host, ":");
    if(sep == 0) {
        return -1;
    }
    *sep++ = 0;
    int port = 0;
    if(sscanf(sep, "%d", &port) != 1) {
        return -1;
    }
    return fill_addr(addr, host, port);
}

static const char *get_string(char *beg, char *end, char *name) {
    int n = strlen(name);
    char q = '\'';
    char Q = '"';
    for(char *p = beg; p < end; p++) {
        if( ((*p == q) || (*p == Q)) &&
            (strncmp(p+1, name, n) == 0) &&
            ((*(p+n+1) == q) || (*(p+n+1) == Q))) {
                for(p += n + 2; p < end; p++) {
                    if(*p == q || *p == Q) {
                        for(char *e = p+1; e < end; e++) {
                            if(*e == q || *e == Q) {
                                *e = 0;
                                return p+1;
                            }

                        }

                    }
                }
        }
    }
    return "";
}


static int parse_msg(char *msg, req_t *req) {
    int rval = -1;
    if(msg == 0 || msg[0] != '{' || msg[strlen(msg)-1] != '}') {
        print("parse_msg start failed");
        return -1;
    }
    char *end = msg + strlen(msg);
    const char *cmd = get_string(msg, end, "cmd");
    const char *mac = get_string(msg, end, "mac");
    const char *pri = get_string(msg, end, "pri");
    const char *pub = get_string(msg, end, "pub");
    //print("cmd %s", cmd);
    //print("mac %s", mac);
    //print("pub %s", pub);
    //print("pri %s", pri);
    if(parse_addr(pub, &req->pub)) {
        print("parse_addr failed");
		return -1;
    }
    if(parse_addr(pri, &req->pri)) {
        print("parse_addr failed");
		return -1;
    }
    for(cmd_t *pcmd = cmds; pcmd < cmds + ncmd; pcmd++) {
        if(strcmp(pcmd->name, cmd) == 0) {
            req->cmd = pcmd->cmd;
			return 0;
        }
    }
    print("Unknown cmd %s", cmd);
	return -1;
}


static int udp_send(int fd, char *cmd, char *buf, int nbuf, struct sockaddr_in *addr) {
    int n = _snprintf(buf, nbuf, CMD_FMT, cmd, my_mac, my_pri, my_pub);
    return sendto(fd, (const char *)buf, strlen(buf),
        0, (const struct sockaddr *) addr, sizeof(*addr));
}

static int udp_recv(int fd, char *buf, size_t nbuf, req_t *req) {
    socklen_t len = sizeof(req->frm);
    //socklen_t before = len;
    bzero(req, sizeof(*req));
    size_t got = recvfrom(fd, buf, nbuf,
            0, (struct sockaddr *)&req->frm, &len);
    //print("len before %d after %d", before, len);
    mark_alive(req->frm);
    buf[got] = 0;
    //print("Server : %s", buf);
    return parse_msg(buf, req);
}

static int udp_action(int fd, req_t *state, char *buf, int nbuf) {
    if(state->cmd == PUNCH) {
        if(is_alive(state->pub) || is_alive(state->pri)) {
            // searching for a udp hole
            print("Send hole to %s:%d",
                addr_ip(&state->pub), addr_port(&state->pub));
            print("Send hole to %s:%d",
                addr_ip(&state->pri), addr_port(&state->pri));
            udp_send(fd, "hole", buf, nbuf, &state->pub);
            udp_send(fd, "hole", buf, nbuf, &state->pri);
        } else {
            print("Punching failed -- timeout");
            state->cmd = 0;
        }
    }

    // never send to someone who haven't heard from in 10 seconds
    if(state->cmd == PLAY) {
        if(is_alive(state->frm)) {
            play = &state->frm;
        } else {
            print("Playing failed -- timeout");
            state->cmd = 0;
            play = 0;
        }
    }
    return 0;
}

static int udp_work() {
    static time_t prev_ping = 0;
    static struct sockaddr_in bind_addr;
    static struct sockaddr_in server_addr;
    static req_t state;

    char buf[128];

    //print("hook");
	int fd = client_fd;
    if(fd == 0) {
        print("open");
		strcpy(my_pri, "0.0.0.0");
        fd = socket(AF_INET, SOCK_DGRAM, 0);
        if(fd <= 0) {
            print("socket error");
            perror("socket");
            return -1;
        }
        fill_addr(&server_addr, SERVER_ADDR, SERVER_PORT);
        fill_addr(&bind_addr, (char *)0, MY_PORT);
        if(bind(fd, (struct sockaddr *) &bind_addr, sizeof(bind_addr))) {
            print("bind error");
            perror("bind");
            return -1;
        }
		client_fd = fd;
    }

    int nread = 0;
    if(can_read(fd) == 0)  {
        //print("no read");
		udp_action(fd, &state, buf, sizeof(buf));
    } else while(can_read(fd)) {
        req_t req;
        //print("read");
		nread++;
        if(udp_recv(fd, buf, sizeof(buf), &req) == 0) {
            if(req.cmd == PING) {
				// ping is from server
                // already marked in alive
                udp_send(fd, "pong", buf, sizeof(buf), &req.frm);
            } else if(req.cmd == PONG) {
				// pong is from server
                if(_snprintf(buf, sizeof(buf), "%s:%d",
                        addr_ip(&req.pub), addr_port(&req.pub))) {
                    return -1;
                }
                if(*my_pub == 0 || strcmp(my_pub, buf)) {
                    print("my_pub = %s", buf);
                	_strncpy(my_pub, buf, sizeof(my_pub));
                }
            } else if(req.cmd == HOLE) {
				// from client
                // The client can find us, can we always find the client?  
                print("Found a hole!");
                mark_alive(req.pub); // overloading TIMOUT
                mark_alive(req.pri);
            } else if(req.cmd == PUNCH) {
				// from server
                // not really alive but used to search for TIMEOUT seconds
                mark_alive(req.pub); // overloading TIMOUT
                mark_alive(req.pri);
                state = req;
            } else if(req.cmd == PLAY) {
				// from client
				//print("Got PLAY!");
                state = req;
            } else {
                print("Unknown cmd %d", req.cmd);
            }
	    	udp_action(fd, &state, buf, sizeof(buf));
        }
    }
    if(nread > 1) {
	    print("Multiple reads %d", nread);
    }

    // watch out for date change
    time_t now = time_now();
    if(now < prev_ping || (now - prev_ping) >= PING_PERIOD) {

		if(strncmp(my_pri, "0.0.0.0", 7) == 0) {
            char my_ip[24];
            get_ip_and_mac(my_ip, my_mac, sizeof(my_ip));
            _snprintf(my_pri, sizeof(my_pri), "%s:%d", my_ip, MY_PORT);
	    	print("my_pri %s", my_pri);
		}

    	//print("ping");
    	udp_send(fd, "ping", buf, sizeof(buf), &server_addr);
        prev_ping = now;
    }
    return 0;
}

/*
static void *udb_thread(void *arg) {
	for(;;) {
		udp_work();
	}
	return 0;
}

public int crb_udp_start() {
	static int init = 0;
	static int pthread_t thread;
	if(init == 0) {
		init = 1;
		int err = pthread_create(&thread, 0, udp_thread, 0);
	}
}
*/

public int crb_udp_send_frame(int fd, char *buf, int n, struct sockaddr_in *addr)
{
	static unsigned char seq = 0;
	static char nal[4] = { 0,0,0,1 };
	int nmax = 1024*63;
	if(memcmp(buf, nal, sizeof(nal)) != 0) {
			print("ERROR: NO NAL AT FRONT");
			return -1;
    }
	int nal_type = buf[4] & 0x1f;
    int nal_ref  = buf[4] & 0x60 >> 5;
	int nal_fbit = buf[4] & 0x80 >> 7;
	print("seq=%d type=%d ref=%d fbit=%d len=%d",
            seq, nal_type, nal_ref, nal_fbit, n);

	int _npack = (n + nmax-1) / nmax;
	if(_npack > 255) {
	    print("ERROR npack>255", _npack);
		return -1;
    }
	unsigned char ipack = 0;
	unsigned char npack = _npack;
	char *end = buf + n;
	// FIX ME: npack calc does not take into account the save buffer
	for(int i = 0; i < npack; i++) {
	//while(buf < end) { // } so this will enever finish
		char save[4];
		int nsend = end - buf;
		unsigned char more = (nsend > nmax);
		if(more) {
			print("BIGASS FRAME breaking up udp packet");
		    nsend = nmax;
		}
		memcpy(save, buf, 4);
		buf[0] = 0;
		buf[1] = seq;
		buf[2] = ipack;
		buf[3] = npack;
		int nsent = sendto(fd, (const char *)buf, nsend,
        	0, (const struct sockaddr *) addr, sizeof(*addr));
		memcpy(buf, save, 4);
		if(nsent != nsend) {
	        print("ERROR send!=sent %d!=%d", nsend, nsent);
			return -1;
		}
		buf += nsend;
		buf -= 4; // the more and seq bytes
		ipack++;
	}
	if(ipack != npack) {
	    print("ERROR ipack!=npack %d!=%d", ipack, npack);
    }
    seq++;
    return n;
}

public int crb_udp_hook(int rval, int chan, void *stream) {
	udp_work();
    if(play && client_fd > 0 && chan == 0 && rval == 0) {
		int err = crb_write_h264(client_fd, play, stream);
    }
}
