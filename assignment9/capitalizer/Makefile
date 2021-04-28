.PHONY: all debug

all:
	gcc -g -Wall -pedantic -std=c99 -o server.out accept_connections.c server.c sock_init.c worker.c -DNDEBUG -lpthread
	gcc -g -Wall -pedantic -std=c99 -o client.out client.c -DNDEBUG
debug:
	gcc -g -Wall -pedantic -std=c99 -o server.out accept_connections.c server.c sock_init.c worker.c -DDEBUG -lpthread
	gcc -g -Wall -pedantic -std=c99 -o client.out client.c -DDEBUG
