CC = gcc
CFLAGS = -c -Wall -pedantic -std=c99 -g

EXE = family.out

.SUFFIXES: .c .h
.PHONY: clean cleanall test memtest

$(EXE): family.o
	$(CC) -o $@ $<
family.o: family.c

clean:
	-rm $(EXE)
cleanall: clean
	-rm *.o
test: $(EXE)
	@echo "Begin test on $(EXE)"
	@bash -e test.sh ./$(EXE)
	@echo "Test passed"
memtest: $(EXE)
	@echo "Begin test on $(EXE)"
	@bash -e memtest.sh ./$(EXE)
	@echo "Test passed"
