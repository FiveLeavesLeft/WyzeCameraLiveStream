#include "common.h"

#define LIB "/system/lib/libimp.so"

static void *sym = 0;
static struct sockaddr_in *p2p_addr = 0;
//static int app_version[4] = {0,0,0,0};
static int V3 = 0;

static int my_write(int fd, void *buf, unsigned n) {
	//static char nal[4] = { 0,0,0,1 };
    if(p2p_addr == 0) {
		/*
		char *p = (char *)buf;
		int nal_type = p[4] & 0x1f;
    	int nal_ref  = p[4] & 0x60 >> 5;
		int nal_fbit = p[4] & 0x80 >> 7;
		//print("nal %d len %d", nal_type, n);
        //print("frame %d", n);
		*/
       return write(fd, buf, n);
    }
	return crb_p2p_send_frame(fd, buf, n, p2p_addr);
}

static int crb_write_h264_v2(int fd, IMPEncoderStream *stream) {
	int n = stream->packCount;
	for(int i = 0; i < n; i++) {
		ssize_t rval = my_write(fd, 
				(void *)stream->pack[i].virAddr,
				stream->pack[i].length);
		if (rval != stream->pack[i].length) {
			return -1;
		}
	}
	return 0;
}

/*
CRB: h264 1 of 1 0 0 0 1 41 len=4966
CRB: h264 1 of 1 0 0 0 1 41 len=1776
CRB: h264 1 of 1 0 0 0 1 41 len=2008
CRB: h264 1 of 1 0 0 0 1 41 len=387
CRB: h264 1 of 3 0 0 0 1 67 len=26
CRB: h264 2 of 3 0 0 0 1 68 len=8
CRB: h264 3 of 3 0 0 0 1 65 len=77699
CRB: h264 1 of 1 0 0 0 1 41 len=2375
*/
static int dump_stats(IMPEncoderStream *stream) {
	int n = stream->packCount;
	unsigned char nal[] = { 0, 0, 0, 1 };
	for(int i = 0; i < n; i++) {
		int len = stream->pack[i].length;
		unsigned char *p = (unsigned char *)stream->pack[i].virAddr;
		int nal_type = p[4] & 0x1f;
		print("stats nal %d %d", nal_type, len);
		if(memcmp(p, nal, sizeof(nal)) != 0) {
			print("LOOK 1! h264 %d of %d %x %x %x %x %x len=%d", i+1, n,
				p[0], p[1], p[2], p[3], p[4], len);
		}
		/*
		//unsigned char x = p[4];
		if(p[4] == 41 || p[4] == 65 || p[4] == 67 || p[4] == 68) {
		} else {
			print("LOOK XX! h264 %d of %d p=%x len=%d",
				i+1, n, p[4], len);
		}
		*/
	}
	return 0;
}

static int get_app_version() {
	FILE *pfile = fopen("/system/bin/app.ver", "r");
	char buf[64];
	if(pfile == 0) {
		print("open /system/bin/app.ver failed");
		return -1;
	}
	while(fgets(buf, sizeof(buf), pfile)) {
		if(strstr(buf, "=")) {
			V3 = strstr(buf, "4.36.") != 0;
		}
		/*
		char *eq = strstr(buf, "=");
		if(eq) {
		    int *p = app_version;
			if(sscanf(eq+1, "%d.%d.%d.%d", p,p+1,p+2,p+3) == 4) {
				print("app version %s", eq+1);
				break;
			}
		}
		*/
	}
	fclose(pfile);
	return 0;
}


static int crb_write_h264_v3(int fd, int param_2) {
  ssize_t sVar2;
  unsigned __n;
  ssize_t sVar3;
  int *piVar5;
  int iVar7;
  unsigned __n_00;
  int iVar8;
  unsigned uVar9;
  int iVar10;
  
  if (*(int *)(param_2 + 0x10) == 0) {
    return 0;
  }
  iVar7 = *(int *)(param_2 + 0xc);
  uVar9 = 0;
  for(;;) {
    iVar10 = uVar9 * 0x20;
    iVar8 = *(int *)(iVar7 + iVar10);
    __n_00 = ((int *)(iVar7 + iVar10))[1];
    __n = *(int *)(param_2 + 8) - iVar8;
    if (__n < __n_00) {
      sVar3 = my_write(fd,(void *)(iVar8 + *(int *)(param_2 + 4)),__n);
      piVar5 = (int *)(*(int *)(param_2 + 0xc) + iVar10);
      sVar2 = my_write(fd,*(void **)(param_2 + 4),(*piVar5 + piVar5[1]) - *(int *)(param_2 + 8));
      iVar7 = *(int *)(param_2 + 0xc);
      sVar3 = sVar3 + sVar2;
      if (sVar3 != *(int *)(iVar7 + iVar10 + 4)) {
	      return -1;
	}
    }
    else {
      sVar3 = my_write(fd,(void *)(iVar8 + *(int *)(param_2 + 4)),__n_00);
      iVar7 = *(int *)(param_2 + 0xc);
      if (sVar3 != *(int *)(iVar7 + iVar10 + 4)) {
        return -1;
      }
    }
    uVar9 = uVar9 + 1;
    if (*(unsigned *)(param_2 + 0x10) <= uVar9) {
      return 0;
    }
  }
}

public void  __attribute__((constructor)) foo(void) {
	print("constructor");
	sym = dlopen(LIB, RTLD_GLOBAL|RTLD_LAZY);
	print("sym = %p", sym);
}

public void _init(void) {
	print("_init");
	foo();
}

public int crb_write_h264(int fd, struct sockaddr_in *addr, void *stream) {
	p2p_addr = addr;
	int err = 0;
	if(V3) {
		err = crb_write_h264_v3(fd, (int)stream);
	} else {
		err = crb_write_h264_v2(fd, (IMPEncoderStream *)stream);
	}
	p2p_addr = 0;
	return err;
}

typedef int (*IMP_Log_Set_Option_t)(int);
typedef int (*IMP_Log_Get_Gption_t)(void);
static IMP_Log_Set_Option_t set = 0;
static IMP_Log_Get_Gption_t get = 0;


public int IMP_Encoder_GetStream(int encChn, IMPEncoderStream *stream, int blockFlag) {
	typedef int (*IMP_Encoder_GetStream_t)(int,IMPEncoderStream*,int);
	static IMP_Encoder_GetStream_t func = 0;
	static int p2p = 0;
	if(sym && func == 0) {
		func = dlsym(sym, "IMP_Encoder_GetStream");
		print("func = %p", func);
		get_app_version();
		char *env = getenv("STREAM_HACK_P2P");
		if(env && crb_p2p_init() == 0) {
			p2p = 1;
		} else {
			crb_tcp_init();
	       	}	
		print("running %s option", p2p ? "p2p" : "local");
		print("running %s option", V3  ? "V3"  : "V2");
	    //set = dlsym(sym, "IMP_Log_Set_Option");
	    //get = dlsym(sym, "IMP_Log_Get_Option");
		//print("get = %p", get);
		//print("set = %p", set);
	}
	if(func) {
		int rval = (func)(encChn,stream,blockFlag);
		if(rval == 0) {
			//dump_stats(stream);
		}
		if(p2p) {
			crb_p2p_hook(rval, encChn, (void *)stream);
		} else {
			crb_tcp_hook(rval, encChn, (void *)stream);
		}
  		return rval;
	}
	print("FAIL GetStream\n");
	return -1;
}

public FILE *crb_file() {
	static FILE *file = 0;
	static int init = 0;
	if(init == 0) {
		init = 1;
		char *path = getenv("STREAM_HACK_OUTPUT");
		if(path == 0 || *path == 0) {
			printf("CRB: No logging.\n");
			return 0;
		}
		if((file = fopen(path, "w")) == 0) {
			file = stdout;
		}
		printf("CRB: logging to %s\n", file == stdout ? "stdout" : path);
		//setbuf(file, 0);
    }
	return file;
}

/*
hack(void *sym) {
	void *func = dlsym(sym, "IMP_Log_Set_Option");
}
*/

#ifdef JUNK
void imp_log_fun(int le, int op, int out, const char* tag, const char* file, int line, const char* func, const char* fmt, ...) {
	va_list ap;
	static FILE *pfile = 0;
		/*
	if(pfile == 0) {
		pfile = fopen("/tmp/imp.log", "w");
		if(pfile && get) {
				int i = get();
				fprintf(pfile, "GET %d\n", i);

				if(set) {
					fprintf(pfile, "SET %d\n", IMP_LOG_LEVEL_WARN);
					(set)(IMP_LOG_LEVEL_WARN);
				}
    	}
	}
	*/
	if(pfile) {
	    fprintf(pfile, "%d %d %d %s:", le, op, out, tag?tag:"");
	    va_start(ap, fmt);
   	    (void) vfprintf(pfile, fmt, ap);
   	    va_end(ap);
	}
}
#endif
