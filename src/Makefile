BIN = /root/mgc/embedded/codebench/bin
PRE = mips-linux-gnu
NM  = $(BIN)/$(PRE)-nm
CC  = $(BIN)/$(PRE)-gcc
LD  = $(BIN)/$(PRE)-ld
DIR = $(shell pwd)
OBJDUMP = $(BIN)/$(PRE)-objdump
CFLAGS = -muclibc -EL -fPIC -nostartfiles
LDFLAGS = -EL -shared

DST	= ../releases/dev

# IMP_ documentation
# https://jmichault.github.io/ipcam-100-dok/en/

it:
	$(CC) $(CFLAGS) -c tcp.c
	$(CC) $(CFLAGS) -c udp.c
	$(CC) $(CFLAGS) -c common.c
	$(LD) $(LDFLAGS) -o libimp.so tcp.o udp.o common.o
	mv libimp.so $(DST)
	cp get $(DST)
	chmod +x $(DST)/get
	ls $(DST)

min:
	$(CC) $(CFLAGS) -fPIC -nostartfiles -c min.c
	$(LD) -EL -shared -o imp.so min.o
	scp imp.so $(SONIC)www/hack.so

dist:
	(cd ../..; tar cvf repo.tar repo; scp repo.tar crb@192.168.0.105:)

clean:
	rm -f *.o *.so

