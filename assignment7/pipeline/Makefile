# Makefile for the pipeline tokenizer
CC                 = gcc -std=c99
CFLAGS            += -g -Wall -O0 -DDEBUG=0
LDFLAGS            = -L .
INCS               = -I .
LIBS               = -pthread -lfifo-queue
TARGET             = pipeline.out

.PHONY: test memtest clean cleanall

$(TARGET): main.c line-reader.c tokenizer.c printer.c libfifo-queue.so
	$(CC) $(INCS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
libfifo-queue.so: fifo-queue.c
	$(CC) $(CFLAGS) $(INCS) -shared -fPIC -o $@ $<
test: $(TARGET)
	/bin/sh -e ./testing.sh ./$(TARGET)
memtest: $(TARGET)
	/bin/sh -e ./memtest.sh ./$(TARGET)
clean:
	-rm -fr *.o *~
cleanall: clean
	-rm -fr $(TARGET)
