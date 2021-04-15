# Makefile for the pipeline tokenizer
BIN                = bin
SRC                = src

CC                 = gcc -std=c99
# these flags cause the program not to print debugging code ( DBG(...) )
CFLAGS            += -g -Wall -O2 -DNDEBUG
# these flags turn it on
DBFLAGS           += -g -Wall -O0 -DDEBUG
LDFLAGS            = -L .
INCS               = -I ./headers
LIBS               = -pthread -lfifo-queue

TARGET             = $(BIN)/pipeline.out
DBTARGET           = $(BIN)/dbg.out

.PHONY: all debug test memtest clean cleanall

all: $(TARGET)
debug: $(DBTARGET)
$(TARGET): $(wildcard $(SRC)/*.c) libfifo-queue.so
	$(CC) $(INCS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
$(DBTARGET): $(wildcard $(SRC)/*.c) libfifo-queue.so
	$(CC) $(INCS) $(DBFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
libfifo-queue.so: fifo-queue/fifo-queue.c
	$(CC) $(CFLAGS) $(INCS) -shared -fPIC -o $@ $<
test: $(TARGET)
	/bin/sh -e ./testing.sh ./$(TARGET)
memtest: $(TARGET)
	/bin/sh -e ./memtest.sh ./$(TARGET)
clean:
	-rm -fr *.o *~
cleanall: clean
	-rm -fr $(TARGET) $(DBTARGET)
