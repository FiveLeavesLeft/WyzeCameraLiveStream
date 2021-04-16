BIN = /root/mgc/embedded/codebench/bin
PRE = mips-linux-gnu
NM  = $(BIN)/$(PRE)-nm
CC  = $(BIN)/$(PRE)-gcc
LD  = $(BIN)/$(PRE)-ld
DIR = $(shell pwd)
OBJDUMP = $(BIN)/$(PRE)-objdump
CFLAGS = -muclibc -EL -fPIC -nostartfiles
LDFLAGS = -EL -shared

DST	= ../releases/current
DST2	= ../releases/dev
DST3	= ../releases/0.0.01
VER	= dev

# IMP_ documentation
# https://jmichault.github.io/ipcam-100-dok/en/

dev:
	$(CC) $(CFLAGS) -c tcp.c
	$(CC) $(CFLAGS) -c p2p.c
	$(CC) $(CFLAGS) -c common.c
	$(LD) $(LDFLAGS) -o libimp.so tcp.o p2p.o common.o
	cp install.sh uninstall.sh libimp.so $(DST)
	cp install.sh uninstall.sh libimp.so $(DST2)
	cp install.sh uninstall.sh libimp.so $(DST3)
	scp install.sh uninstall.sh libimp.so crb@shell.sonic.net:www/dev
	#rm *.o libimp.so

current:
	make dev
	scp install.sh uninstall.sh libimp.so crb@shell.sonic.net:www/current

dist:
	(cd ../..; tar cvf repo.tar repo; scp repo.tar crb@192.168.0.105:)

clean:
	rm -f *.o *.so

