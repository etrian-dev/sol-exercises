CC = gcc
CFLAGS = -Wall -pedantic -std=c99 -O0 -g

EXE1 = write-mat.out
EXE2 = read-cmp-matr.out

DUMP1 = mat_dump.txt
DUMP2 = mat_dump.dat
SIZE = 5

.PHONY: cleanall clean test

#tests the executables on a size of 100
test: $(EXE1) $(EXE2)
	@echo "Testing $(EXE1) and $(EXE2)"
	./$(EXE1) $(SIZE)
	@#test if the matrices read are equal: output should be EQUAL
	./$(EXE2) $(DUMP1) $(DUMP2) $(SIZE) > output.txt
	@#modify the matrix: output should be DIFFERENT
	@nano $(DUMP1)
	./$(EXE2) $(DUMP1) $(DUMP2) $(SIZE) >> output.txt
	@echo "EQUAL\nDIFFERENT" > correct.txt
	@diff output.txt correct.txt
	@echo "Test passed"
	-rm correct.txt output.txt $(DUMP1) $(DUMP2)

clean:
	-rm ./*.out
cleanall: clean
	-rm ./*.o ./*.txt ./*.dat
