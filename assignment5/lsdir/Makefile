# Makefile to build the executables and execute tests
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g
DBFLAGS = -DDEBUG=1
# the path to the shared library: in this case it's embedded in the executable
LIBDIR = ./lib
LDFLAGS = -Wl,-rpath=$(LIBDIR) -L $(LIBDIR)
LDLIBS = -lutilities

EXE = bin/lsdir.out
DBG = bin/lsdir-dbg.out
SRC = src
OBJS = objs
HEADERS = -I ./headers


.PHONY: all debug clean cleanall test testdbg

all: $(EXE)


$(EXE): $(OBJS)/main.o $(LIBDIR)/libutilities.so
	$(CC) $(CFLAGS) $(HEADERS) $(LDFLAGS) -o $@ $< $(LDLIBS)

$(LIBDIR)/libutilities.so: $(OBJS)/utilities.o $(OBJS)/lsdir.o
	$(CC) -shared -o $@ $^
	rm $^

$(OBJS)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(HEADERS) -c -fPIC -o $@ $<
	
debug: $(wildcard $(SRC)/*.c)
	$(CC) $(CFLAGS) $(DBFLAGS) $(HEADERS) -o $(DBG) $^

clean:
	-rm $(EXE)
cleanall: clean
	-rm $(wildcard $(OBJS)/*.o)
# shared lib path embedded, no need to specify LD_LIBRARY_PATH
test: $(EXE)
	@echo "Testing $(EXE)"
	@/bin/bash -e test.sh ./$(EXE)
	@echo "Test passed"
testdbg: $(DBG)
	@echo "Debugging $(DBG)"
	@/bin/bash -e test.sh ./$(DBG)
	@echo "Debug passed"
