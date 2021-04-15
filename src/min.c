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

#include "imp/imp_isp.h"
#include "imp/imp_system.h"
#include "imp/imp_log.h"
#include "imp/imp_framesource.h"
#include "imp/imp_encoder.h"

#define LIB "/system/lib/libimp.so"

static void *sym = 0;

void  __attribute__((constructor)) foo(void) {
	printf("CRBMIN constructor");
	sym = dlopen(LIB, RTLD_GLOBAL|RTLD_LAZY);
	printf("CRBMIN sym = %p", sym);
}

void _init(void) {
	printf("CRBMIN _init");
	foo();
}

int IMP_Encoder_GetStream(int encChn, IMPEncoderStream *stream, int blockFlag) {
	typedef int (*IMP_Encoder_GetStream_t)(int,IMPEncoderStream*,int);
	static IMP_Encoder_GetStream_t func = 0;
	if(sym && func == 0) {
		func = dlsym(sym, "IMP_Encoder_GetStream");
		printf("func = %p", func);
	}
	if(func) {
		return (func)(encChn,stream,blockFlag);
	}
	printf("FAIL GetStream\n");
	return -1;
}
