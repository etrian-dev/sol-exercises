EXE = simple-wc.out

.PHONY: clean test

test: $(EXE)
	@echo "Testing $(EXE) (LEFT SIDE) and wc (RIGHT SIDE)"
#executes my clone (no option passed => -w -l implicit) and wc on all files under tests/
	@./$(EXE) -w -l tests/* > myout.txt
	@wc -w -l tests/* > correct.txt
#diffs the outputs side by side: mine on the left and wc's on the right
	@-diff -y myout.txt correct.txt
	@rm myout.txt correct.txt
	@echo "Test passed"
	
clean:
	-rm ./*.o ./*.out ./*.txt
