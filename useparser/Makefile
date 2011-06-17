
CC=gcc
# CFLAGS=-c -Wall -pedantic -O3 -std=c99
CFLAGS=-c -Wall -pedantic -std=c99 -g

all: useparser

useparser: main.o yenc.o zlib.o parser.o md5.o common.o
	$(CC) main.o yenc.o zlib.o parser.o md5.o common.o -lz -o useparser

main.o:
	$(CC) $(CFLAGS) src/main.c

yenc.o:
	$(CC) $(CFLAGS) src/yenc.c

zlib.o:
	$(CC) $(CFLAGS) src/zlib.c

parser.o:
	$(CC) $(CFLAGS) src/parser.c

md5.o:
	$(CC) $(CFLAGS) src/md5.c

common.o:
	$(CC) $(CFLAGS) src/common.c

clean:
	@rm -rf useparser *.o

