CC = gcc
CFLAGS = -g -Wall -pedantic -std=c99
OPTFLAGS = -O2

CLIENT = bc-client
SERVER = bc-serv

.PHONY: all debug

debug:
	$(CC) $(CFLAGS) -o $(SERVER).out $(SERVER).c -DDEBUG
	$(CC) $(CFLAGS) -o $(CLIENT).out $(CLIENT).c -DDEBUG
	-rm -rf s1
all:
	$(CC) $(CFLAGS) $(OPTFLAGS) -o dbg-$(SERVER).out $(SERVER).c -DNDEBUG
	$(CC) $(CFLAGS) $(OPTFLAGS) -o dbg-$(CLIENT).out $(CLIENT).c -DNDEBUG
	-rm -rf s1
