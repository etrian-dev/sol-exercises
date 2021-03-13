# Makefile per compilare e testare i file contenuti in questa directory
CC		=  gcc
CFLAGS          = -Wall -pedantic -std=c99 -g

# l'eseguibile da generare è myprog
EXE = ./bin/myprog
MYLIB = libbt.so
INCLUDES = -I ./include
LIBS = -Wl,-rpath=./lib -L ./lib -lbt

.PHONY: test1 clean cleanall

$(EXE): src/myprog.o lib/$(MYLIB)
	$(CC) $(CFLAGS) -o $(EXE) $< $(LIBS)
lib/$(MYLIB): src/binarytree.o
	$(CC) -shared -o $@ $<
src/binarytree.o: src/binarytree.c include/binarytree.h include/utils.h
	$(CC) $(CFLAGS) $(INCLUDES) -fPIC -c -o $@ $<
src/myprog.o: src/myprog.c include/binarytree.h include/utils.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<
# testa il programma con determinati argomenti e confronta output con quello del
# file nella dir testfile
test1: $(EXE)
	@$(EXE) 5 -2 28 > out
	@diff out testfile/test1-out_atteso.txt
	@echo "test1 OK"
	@rm out
clean:
	-rm $(EXE)
cleanall: clean
	-rm lib/libbt.so src/*.o
