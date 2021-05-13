CC = gcc
CFLAGS = -g -Wall -pedantic -std=c99
HEADERS = -I .
LIBS = -lpthread

MACROS = -D_POSIX_C_SOURCE=199506L -DNDEBUG

CLIENT = client.out
SERVER = server.out

.PHONY: all clean memtest

all: $(CLIENT) $(SERVER)
$(CLIENT): client.c
	$(CC) $(CFLAGS) $(HEADERS) -o $@ $< $(MACROS)
$(SERVER): server.c sock_init.c worker.c handler.c thread_termsig.c queue.c util.c
	$(CC) $(CFLAGS) $(HEADERS) -o $@ $^ $(LIBS) $(MACROS)
clean:
	-rm -fr $(CLIENT) $(SERVER)
memtest: $(CLIENT) $(SERVER)
	valgrind --leak-check=full ./$(SERVER)
