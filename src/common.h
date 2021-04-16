#ifndef __HACK_H__
#define __HACK_H__
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>   
#include <stdarg.h>  
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <sys/stat.h> 
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <net/if.h>

#include "imp/imp_isp.h"
#include "imp/imp_system.h"
#include "imp/imp_log.h"
#include "imp/imp_framesource.h"
#include "imp/imp_encoder.h"

#define public /* nothing */
public int crb_p2p_init();
public int crb_tcp_init();
public int crb_p2p_hook(int, int, void *);
public int crb_tcp_hook(int, int, void *);
public int crb_write_h264(int fd, struct sockaddr_in *addr, void *stream);
public FILE *crb_file();
public int crb_p2p_send_frame(int fd, char *buf, int n, struct sockaddr_in *addr);

// TODO: truncate at some size
static void print(char *fmt, ...) {
	FILE *file = crb_file();
	va_list ap;
	if(file) {
		fputs("CRB: ", file);
		va_start(ap, fmt);
   		(void) vfprintf(file, fmt, ap);
   		va_end(ap);
		fputs("\n", file);
		fflush(file);
	}
}

static int exists(char *path) {
	struct stat sbuf;
	return lstat(path, &sbuf) == 0;
}

#endif
