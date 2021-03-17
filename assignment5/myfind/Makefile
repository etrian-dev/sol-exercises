# Makefile to build the executables and execute tests

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g
DBFLAGS = -DDEBUG=1

EXE = bin/myfind.out
DBG = bin/myfind-dbg.out
SRC = src
OBJS = objs
HEADERS = -I ./headers

.PHONY: all dbg clean cleanall test testdbg


all: $(EXE)

$(EXE): $(OBJS)/myfind.o $(OBJS)/utilities.o $(OBJS)/explore-dir.o
	$(CC) $(CFLAGS) $(HEADERS) -o $@ $^

$(OBJS)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(HEADERS) -c -o $@ $<

dbg: $(DBG)

$(DBG): $(wildcard $(SRC)/*.c)
	$(CC) $(CFLAGS) $(HEADERS) $(DBFLAGS) -o $@ $^

	
clean:
	-rm ./bin/*.out
cleanall: clean
	-rm $(wildcard $(OBJS)/*.o)
test: $(EXE)
	@echo "Testing $(EXE)"
	@/bin/bash -e test.sh ./$(EXE) 
	@echo "Test passed"
testdbg: $(DBG)
	@echo "Debug test $(DBG)"
	@/bin/bash -e test.sh ./$(DBG)
	@echo "Test passed"
#memcheck: $(EXE)
#	@echo "Memory checking with valgrind on $(EXE) and "
#	@/bin/sh -e memcheck.sh ./$(EXE) 
#	@echo 'Valgrind output written to memcheck*.log'
