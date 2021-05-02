CC = gcc
CFLAGS = -g -Wall -pedantic -std=c99
HEADERS = -I .
SERVER = server.out
CLIENT = client.out

.PHONY: all debug

all: $(SERVER) $(CLIENT)
debug: server.c sock_init.c client.c
	$(CC) $(CFLAGS) $(HEADERS) -o $(SERVER) server.c sock_init.c -DDEBUG
	$(CC) $(CFLAGS) $(HEADERS) -o $(CLIENT) client.c -DDEBUG
$(SERVER): server.c sock_init.c
	$(CC) $(CFLAGS) $(HEADERS) -o $(SERVER) $^ -DNDEBUG
$(CLIENT): client.c
	$(CC) $(CFLAGS) $(HEADERS) -o $(CLIENT) $< -DNDEBUG
clean: $(SERVER) $(CLIENT)
	-rm -rf $(SERVER) $(CLIENT)
cleanall: clean
	-rm -rf ./sock
