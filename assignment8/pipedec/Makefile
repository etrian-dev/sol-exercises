CC = gcc -std=c99
CFLAGS = -g -Wall -pedantic

PIPEDEC = pipedec.out
DEC = dec.out

.PHONY: all clean cleanall test

all: $(PIPEDEC) $(DEC)
$(PIPEDEC): pipedec.c
	$(CC) $(CFLAGS) -o $@ $<
$(DEC): dec.c
	$(CC) $(CFLAGS) -o $@ $<
clean:
	-rm -rf *.o
cleanall: clean
	-rm -rf *.out
test: all
	/bin/sh -e testing.sh ./$(PIPEDEC)
