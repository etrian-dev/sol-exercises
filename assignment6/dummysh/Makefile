CC = gcc
CFLAGS = -Wall -pedantic -std=c99 -g

EXE = dummysh.out

.SUFFIXES: .c .h
.PHONY: clean cleanall test memtest

$(EXE): dummysh.o
	$(CC) $(CFLAGS) -o $@ $<

dummysh.o: dummysh.c

clean:
	-rm $(EXE)
cleanall: clean
	-rm *.o
test: $(EXE)
	@echo "Begin test on $(EXE)"
	@./$(EXE) < testing/input0.txt
	@echo "Test passed"
memtest: $(EXE)
	@echo "Begin test on $(EXE)"
	@valgrind --leak-check=full ./$(EXE) < testing/input0.txt
	@echo "Test passed"
