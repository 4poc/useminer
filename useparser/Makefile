CC=gcc
CFLAGS=-Wall -std=c99 -D_XOPEN_SOURCE -D_POSIX_C_SOURCE=200112L
LDFLAGS=-lz -lgmp
SRC_FILES=main.c common.c config.c parse.c file.c cache.c md5.c yenc.c zlib.c 
DIR=./src
SRC=$(addprefix $(DIR)/,$(SRC_FILES))
OBJ=$(SRC:.c=.o)
    
all: useparser
clang: CC=clang
clang: useparser
debug: CFLAGS+=-DENABLE_DEBUG -g
debug: useparser

useparser: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

$(DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	@rm -rf useparser src/*.o

