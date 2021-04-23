# Makefile for pipebc
CC               = gcc -std=gnu11
CFLAGS		    += -Wall -O2 -DNDEBUG
DBFLAGS         += -g -Wall -O0 -DDEBUG

TARGET		     = bin/pipebc.out
DBG              = bin/debug.out

.PHONY: all debug clean cleanall test

all: $(TARGET)
debug: src/main.c
	$(CC) $(DBFLAGS) -o $(DBG) $<
$(TARGET): src/main.c
	$(CC) $(CFLAGS) -o $(TARGET) $<
clean:
	-rm -fr $(wildcard objs/*.o) *~
cleanall: clean
	-rm -fr $(TARGET)
test: $(TARGET)
	./$(TARGET) < testcases/input0.txt
