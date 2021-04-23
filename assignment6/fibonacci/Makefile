CC                 = gcc -std=c99
CFLAGS            += -g -Wall -O3
INCS               = -I .
TARGET             = fib

.PHONY: all clean cleanall test

%: %.c
	$(CC) $(INCS) $(CFLAGS) -o $@ $<

all: $(TARGET)
clean:
	-rm -fr *.o *~
cleanall: clean
	-rm -fr $(TARGET)
