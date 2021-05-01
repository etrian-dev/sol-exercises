HEADERS = -I .

.PHONY: all debug

all:
	gcc -g -Wall -pedantic -std=c99 $(HEADERS) -o server.out server.c sock_init.c -DNDEBUG -lpthread
	gcc -g -Wall -pedantic -std=c99 $(HEADERS) -o client.out client.c -DNDEBUG
debug:
	gcc -g -Wall -pedantic -std=c99 $(HEADERS) -o server.out server.c sock_init.c -DDEBUG -lpthread
	gcc -g -Wall -pedantic -std=c99 $(HEADERS) -o client.out client.c -DDEBUG
