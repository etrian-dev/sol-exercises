# Makefile to build the executables and execute tests

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g
DBFLAGS = -DDEBUG=1

SYSCALL_CP = bin/mycp-sys.out
LIBC_CP = bin/mycp-libc.out
SRC = src
OBJS = objs

.PHONY: all clean cleanall test memcheck

all: $(SYSCALL_CP) $(LIBC_CP)

$(SYSCALL_CP): $(OBJS)/mycp-sys.o
	$(CC) $(CFLAGS) -o $@ $<

db-$(SYSCALL_CP): $(OBJS)/mycp-sys.o
	$(CC) $(CFLAGS) $(DBFLAGS) -o $@ $<
$(LIBC_CP): $(OBJS)/mycp-libc.o
	$(CC) $(CFLAGS) -o $@ $<
$(OBJS)/mycp-sys.o: $(SRC)/mycp-sys.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(OBJS)/mycp-libc.o: $(SRC)/mycp-libc.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm ./bin/*.out
cleanall: clean
	-rm $(OBJS)/*.o ./output/* ./memcheck.log
test: $(SYSCALL_CP)
	@echo "Testing $(SYSCALL_CP) and $(LIBC_CP)"
	@/bin/bash -e test.sh ./$(SYSCALL_CP) ./$(LIBC_CP)
	@echo "Test passed"
memcheck: $(SYSCALL_CP)
	@echo "Memory checking with valgrind on $(SYSCALL_CP) and $(LIBC_CP)"
	@/bin/sh -e memcheck.sh ./$(SYSCALL_CP) ./$(LIBC_CP)
	@echo 'Valgrind output written to memcheck*.log'
