CC = gcc
CFLAGS = -g -Wall -pedantic -std=c99

STRTOUPPER = mystrtoupper.out

.PHONY: all

all: $(STRTOUPPER)

$(STRTOUPPER): mystrtoupper.c
	$(CC) $(CFLAGS) -o $@ $<
