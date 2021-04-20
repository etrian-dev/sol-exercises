# Makefile for the concurrent messages pipeline
CC          = gcc -std=c99
CFLAGS		+= -Wall -O2 -DNDEBUG
DBFLAGS     += -g -Wall -O0 -DDEBUG
LDFLAGS		= -L ./libs -Wl,-rpath=./libs
INCS		= -I ./headers
LIBS		= -pthread -lfifo-queue

TARGET		= ./bin/messqueue.out
DBG         = ./bin/debug.out

.PHONY: all debug clean cleanall memcheck

all: $(TARGET)
debug: libs/libfifo-queue.so $(wildcard src/*.c)
	$(CC) $(DBFLAGS) $(INCS) $(LDFLAGS) -o $(DBG) $(wildcard src/*.c) $(LIBS)
$(TARGET): libs/libfifo-queue.so $(wildcard src/*.c)
	$(CC) $(CFLAGS) $(INCS) $(LDFLAGS) -o $@ $(wildcard src/*.c) $(LIBS)
# commands to create the shared lib for the queue
libs/libfifo-queue.so: objs/fifo-queue.o
	$(CC) $(CFLAGS) $(INCS) -shared -o $@ $<
objs/fifo-queue.o: fifo-queue/fifo-queue.c
	$(CC) $(CFLAGS) $(INCS) -c -fPIC -o $@ $<
clean:
	-rm -fr $(wildcard objs/*.o) $(wildcard libs/*.so) *~
cleanall: clean
	-rm -fr $(TARGET)
memcheck: debug
	/bin/sh -e ./memtest.sh ./$(DBG)
