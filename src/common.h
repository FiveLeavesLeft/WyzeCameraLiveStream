#ifndef __HACK_H__
#define __HACK_H__
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>     // malloc etc
#include <stdarg.h>     // malloc etc
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <sys/stat.h> 
#include <arpa/inet.h>  // inet_addr
#include <errno.h>
#include <pthread.h>
#include <net/if.h>
//#include <execinfo.h>

#include "imp/imp_isp.h"
#include "imp/imp_system.h"
#include "imp/imp_log.h"
#include "imp/imp_framesource.h"
#include "imp/imp_encoder.h"

#define public /* nothing */
public int crb_udp_hook(int, int, void *);
public int crb_tcp_hook(int, int, void *);
public int crb_write_h264(int fd, struct sockaddr_in *addr, void *stream);
public FILE *crb_file();
public int crb_udp_send_frame(int fd, char *buf, int n, struct sockaddr_in *addr);

// TODO: truncate at some size
static void print(char *fmt, ...) {
	FILE *file = crb_file();
	va_list ap;
	fputs("CRB: ", file);
	va_start(ap, fmt);
   	(void) vfprintf(file, fmt, ap);
   	va_end(ap);
	fputs("\n", file);
	fflush(file);
}

#endif

