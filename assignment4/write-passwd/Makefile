EXE = write-passwd.out
OUTFILE = usernames.txt

.PHONY: clean test
#tests the program by dumping the results in OUTFILE and displaying it
test: $(EXE)
	@echo "Testing $(EXE), output in $(OUTFILE)"
	@./$(EXE) $(OUTFILE)
	@echo "Test passed"

clean:
	-rm ./*.o ./*.out ./*.txt
