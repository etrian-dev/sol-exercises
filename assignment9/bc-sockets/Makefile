CC = gcc
CFLAGS = -g -Wall -pedantic -std=c99
HEADERS = -I .

DBG = -DDEBUG

CLIENT = bcclient.out
SERVER = bcserv.out

.PHONY: all debug clean cleanall

all: $(CLIENT) $(SERVER)
debug: bc-client.c bc-serv.c child.c
	$(CC) $(CFLAGS) $(HEADERS) -o $(CLIENT) bc-client.c $(DBG)
	$(CC) $(CFLAGS) $(HEADERS) -o $(SERVER) bc-serv.c child.c $(DBG)
$(CLIENT): bc-client.c
	$(CC) $(CFLAGS) $(HEADERS) -o $(CLIENT) $<
$(SERVER): bc-serv.c child.c
	$(CC) $(CFLAGS) $(HEADERS) -o $(SERVER) $^
clean: $(CLIENT) $(SERVER)
	-rm -fr $(CLIENT) $(SERVER)
cleanall: clean
	-rm -fr sock
