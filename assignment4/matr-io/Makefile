#the executables are expected to have these filenames
WRITE_MAT = write-mat.out
CMP_MAT = read-cmp-matr.out
#the files used to dump the results are expected to be these
DUMP1 = mat_dump.txt
DUMP2 = mat_dump.dat
#size is arbitrary, but must be the same specified when the dumps were created
SIZE = 5

.PHONY: cleanall clean test

#tests the executables on a matrix of size of 100
test: $(WRITE_MAT) $(CMP_MAT)
	@echo "Testing $(WRITE_MAT) and $(CMP_MAT)..."
	@./$(WRITE_MAT) $(SIZE)
	@./$(CMP_MAT) $(DUMP1) $(DUMP2) $(SIZE) > output.txt
	@nano $(DUMP1)
	@./$(CMP_MAT) $(DUMP1) $(DUMP2) $(SIZE) >> output.txt
	@echo "EQUAL\nDIFFERENT" > correct.txt
	@diff output.txt correct.txt
	@echo "Test passed"
	@rm correct.txt output.txt $(DUMP1) $(DUMP2)
clean:
	-rm ./*.out
cleanall: clean
	-rm ./*.o ./*.txt ./*.dat
