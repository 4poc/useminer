# CC=clang
# CFLAGS=-c -Wall -pedantic -std=c99 -D_XOPEN_SOURCE -D_POSIX_C_SOURCE=200112L 

# gcc:
CC=gcc
CFLAGS=-c -Wall -pedantic -std=c99 -D_XOPEN_SOURCE -D_POSIX_C_SOURCE=200112L -g
# -g 	for debugging

all: useparser

useparser: main.o yenc.o zlib.o storage.o parser.o config.o overview.o binary.o md5.o common.o
	$(CC) main.o yenc.o zlib.o storage.o parser.o config.o overview.o binary.o md5.o common.o -lz -lgmp -o useparser

main.o:
	$(CC) $(CFLAGS) src/main.c

yenc.o:
	$(CC) $(CFLAGS) src/yenc.c

zlib.o:
	$(CC) $(CFLAGS) src/zlib.c

parser.o:
	$(CC) $(CFLAGS) src/parser.c

storage.o:
	$(CC) $(CFLAGS) src/storage.c

config.o:
	$(CC) $(CFLAGS) src/config.c

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

