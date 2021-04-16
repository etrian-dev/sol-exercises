# Makefile for the pipeline tokenizer

#define my dirs
SRC         = src
LIBDIR      = libs
BIN         = bin

CC          = gcc -std=c99
CFLAGS		+= -g -Wall -O0 -DDEBUG=0
LDFLAGS		= -L ./libs
INCS		= -I ./fifo-queue ./icl_hash
LIBS		= -pthread -lfifo-queue -licl_hash

TARGET		= $(BIN)/dict-pipe.out

.PHONY: test memtest clean cleanall benchmark

$(TARGET): $(wildcard $(SRC)/*.c) $(LIBDIR)/libicl_hash.so $(LIBDIR)/libfifo-queue.so
	$(CC) $(INCS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
# commands to create shared libraries
$(LIBDIR)/libicl_hash.so: icl_hash/icl_hash.c
	$(CC) $(CFLAGS) $(INCS) -shared -fPIC -o $@ $<
$(LIBDIR)/libfifo-queue.so: fifo-queue/fifo-queue.c
	$(CC) $(CFLAGS) $(INCS) -shared -fPIC -o $@ $<

test: $(TARGET)
	/bin/sh -e ./testing.sh ./$(TARGET)
memtest: $(TARGET)
	/bin/sh -e ./memtest.sh ./$(TARGET)
benchmark: $(TARGET)
	/bin/sh -e ./benchmark.sh ./$(TARGET)
clean:
	-rm -fr $(wildcard $(OBJS)/*.o) $(wildcard $(LIBDIR)/*.so) *~
cleanall: clean
	-rm -fr $(TARGET)
