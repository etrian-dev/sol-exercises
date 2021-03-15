# Makefile to build the executables and execute tests

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g
DBFLAGS = -DDEBUG=1

EXE = bin/myfind.out
SRC = src
OBJS = objs

.PHONY: all debug clean cleanall test testdbg

all: $(EXE)

debug: $(EXE)-dbg

$(EXE): $(OBJS)/myfind.o
	$(CC) $(CFLAGS) -o $@ $<

$(EXE)-dbg: $(OBJS)/myfind-dbg.o
	$(CC) $(CFLAGS) -o $@ $<
	
$(OBJS)/myfind.o: $(SRC)/myfind.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJS)/myfind-dbg.o: $(SRC)/myfind.c
	$(CC) $(CFLAGS) $(DBFLAGS) -c -o $@ $<
clean:
	-rm ./bin/*.out
cleanall: clean
	-rm $(OBJS)/*.o ./memcheck.log
test: $(EXE)
	@echo "Testing $(EXE)"
	@/bin/bash -e test.sh ./$(EXE) 
	@echo "Test passed"
testdbg: $(EXE)-dbg
	@echo "Debug test $(EXE)-dbg"
	@/bin/bash -e test.sh ./$(EXE)-dbg 
	@echo "Test passed"
#memcheck: $(EXE)
#	@echo "Memory checking with valgrind on $(EXE) and "
#	@/bin/sh -e memcheck.sh ./$(EXE) 
#	@echo 'Valgrind output written to memcheck*.log'
