EXE = write-passwd.out
OUTFILE = output.txt

.PHONY: cleanall clean test

#tests the program by running it and then comparing the result with the given command
test: $(EXE)
	@echo "Testing $(EXE)..."
	@./$(EXE) $(OUTFILE)
	@./$(EXE) $(OUTFILE)
	@cut -d ':' -f 1 /etc/passwd | diff - $(OUTFILE)
	@rm $(OUTFILE)
	@echo "Test passed"
clean:
	-rm ./*.out
cleanall: clean
	-rm ./*.o ./*.txt
