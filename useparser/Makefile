
CC=gcc
# CFLAGS=-c -Wall -pedantic -O3 -std=c99
CFLAGS=-c -Wall -pedantic -std=c99 -D_XOPEN_SOURCE -D_POSIX_C_SOURCE=200112L -g

all: useparser

useparser: main.o yenc.o zlib.o overview.o binary.o md5.o common.o
	$(CC) main.o yenc.o zlib.o overview.o binary.o md5.o common.o -lz -o useparser

main.o:
	$(CC) $(CFLAGS) src/main.c

yenc.o:
	$(CC) $(CFLAGS) src/yenc.c

zlib.o:
	$(CC) $(CFLAGS) src/zlib.c

overview.o:
	$(CC) $(CFLAGS) src/overview.c

binary.o:
	$(CC) $(CFLAGS) src/binary.c

md5.o:
	$(CC) $(CFLAGS) src/md5.c

common.o:
	$(CC) $(CFLAGS) src/common.c

clean:
	@rm -rf useparser *.o

