#the subdirectories containing files to be made
DIR1 = write-passwd
DIR2 = matr-io
DIR3 = simple-wc
#the executables created in the respective directories
ES1 = write-passwd.out
ES2-1 = write-mat.out
ES2-2 = read-cmp-matr.out
ES3 = simple-wc.out

#C compiler options
CC = gcc
#includes used to make the files in DIR3 are also in the flags, so that implicit
#rules can be used to compile the object files
CFLAGS = -Wall -pedantic -std=c99 -O0 -g -I ./$(DIR3)

#these are the phony targets
.PHONY: cleanall clean test all

#phony target to make all subdirectories of the project
all: $(DIR1)/$(ES1) $(DIR2)/$(ES2-1) $(DIR2)/$(ES2-2) $(DIR3)/$(ES3)

#compile object files into executables
$(DIR1)/$(ES1): $(DIR1)/write-passwd.o
	$(CC) $(CFLAGS) -o $@ $<
$(DIR2)/$(ES2-1): $(DIR2)/write-mat.o
	$(CC) $(CFLAGS) -o $@ $<
$(DIR2)/$(ES2-2): $(DIR2)/read-cmp-matr.o
	$(CC) $(CFLAGS) -o $@ $<
$(DIR3)/$(ES3): $(DIR3)/simple-wc.o $(DIR3)/parse-opt.o
	$(CC) $(CFLAGS) -o $@ $^
#compile source files into objects
#includes are added to CFLAGS, so don't worry about them
$(DIR1)/write-passwd.o: $(DIR1)/write-passwd.c
$(DIR2)/write-mat.o: $(DIR2)/write-mat.c
$(DIR2)/read-cmp-matr.o: $(DIR2)/read-cmp-matr.c
$(DIR3)/simple-wc.o: $(DIR3)/simple-wc.c $(DIR3)/parse-opt.h
$(DIR3)/parse-opt.o: $(DIR3)/parse-opt.c $(DIR3)/parse-opt.h

cleanall: clean
	cd $(DIR1) && $(MAKE) cleanall
	cd $(DIR2) && $(MAKE) cleanall
	cd $(DIR3) && $(MAKE) cleanall
clean:
	cd $(DIR1) && $(MAKE) clean
	cd $(DIR2) && $(MAKE) clean
	cd $(DIR3) && $(MAKE) clean
test: all
	cd $(DIR1) && $(MAKE) test
	cd $(DIR2) && $(MAKE) test
	cd $(DIR3) && $(MAKE) test
