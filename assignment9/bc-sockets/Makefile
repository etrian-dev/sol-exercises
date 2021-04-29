CC = gcc
CFLAGS = -g -Wall -pedantic -std=c99

DBG = -DDEBUG

CLIENT = bc-client
SERVER = bc-serv

.PHONY: all

all: $(CLIENT) $(SERVER)
$(CLIENT):
	$(CC) $(CFLAGS) -o $(CLIENT).out $(CLIENT).c $(DBG)
$(SERVER):
	$(CC) $(CFLAGS) -o $(SERVER).out $(SERVER).c child.c $(DBG)
