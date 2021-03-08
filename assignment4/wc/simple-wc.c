// Simple wc clone (man 1 wc), limited to the -w and -l options (word and line counts respectively)
// The counts are maintained on a per-file basis, but a total count is added if more than
// one file is supplied as an argument to the program
// NOTE:  Does not work on binary files because it uses fgets

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> // for isspace & co.
#include <unistd.h> // for getopt

// Recognized options
#define OPSTRING "wl"
#define HELPSTRING "h"
// useful macros to indicate if counting is turned on or off
#define COUNT_ON 1
#define COUNT_OFF !COUNT_ON
// defines the buffer increment size (alloc'd to read the file)
#define BUFSZ 1000

// declarations because c99 gets bitchy
int getopt(int argc, char **argv, const char *opstring);
extern int optind;
extern int opterr;
// prints the help message to stdout
void print_usage(const char *program);
int process_buf(const char *buf, long int *nlines, long int *nwords);

int main(int argc, char **argv) {
	// if no args are provided, quit with usage message
	if(argc == 1) {
		print_usage(argv[0]);
		return EXIT_SUCCESS;
	}
	// search for the help option (-h)
	opterr = 0;
	int option;
	while((option = getopt(argc, argv, HELPSTRING)) != -1) {
		if(option == 'h') {
			// help option recognized: print usage message and exit with success
			print_usage(argv[0]);
			return EXIT_SUCCESS;
		}
	}
	// otherwise search for the other options
	int count_words = COUNT_OFF;
	int count_lines = COUNT_OFF;

	opterr = 1;
	optind = 1;
	while((option = getopt(argc, argv, OPSTRING)) != -1) {
		switch(option) {
		case 'w': // recognized the -w option: the program will count words
			count_words = COUNT_ON;
			break;
		case 'l': // recognized the -w option: the program will count lines
			count_lines = COUNT_ON;
		/*
		Error handling?
		*/
		}
	}
	// print usage if no option is specified
	if((count_words|count_lines) == 0) {
		print_usage(argv[0]);
		return EXIT_SUCCESS;
	}

	// now open the file(s) and read them line by line
	// maintain the requested counts
	long int n_words = 0;
	long int n_lines = 0;
	long int tot_words = 0;
	long int tot_lines = 0;

	// optind is the first non-option argument to the program (permuted to be after all options)
	// so the remaining part of argv is scanned to find all the filenames
	int i = optind;
	while(argv[i] != NULL) {
	#ifdef DEBUG
		printf("argv[%d] = %s\n", i, argv[i]);
	#endif
		FILE *ifile = fopen(argv[i], "rb");
		// if (for some reason) a file cannot be opened, keep running on the remaining ones
		if(!ifile) {
			perror(argv[i]);
		}

		// read the file line by line and update the counts
		n_words = 0;
		n_lines = 0;
		int word_longer_than_BUFSZ = 0; // this should be self-explanatory, but...
		// when counting lines & words a line/word might span across different reads
		// and thus this is needed to keep track of words unfinished in the previous buffer

		char *line = calloc(BUFSZ, sizeof(char));
		if(!line) {
			perror("Calloc error in __FILE__ at line __LINE__");
			exit(EXIT_FAILURE);
		}

		while(feof(ifile) == 0) {
			// reads BUFSZ characters from the file (at most)
			char *s = fgets(line, BUFSZ, ifile);
			if(!s && feof(ifile) == 0) {
				perror("Error reading data from file");
				exit(EXIT_FAILURE);
			}
			int return_code = process_buf(line, &n_lines, &n_words);
			// if the previous buffer(s) did not end the word and this did, add it
			if(word_longer_than_BUFSZ == 1 && return_code == 0) {
				n_words++;
			}

			#ifdef DEBUG
			printf("line: \"%s\"\n#lines = %ld\n#words = %ld\n", line, n_lines, n_words);
			#endif

			// reset the line to zeros
			line = memset(line, 0, BUFSZ * sizeof(char));
		}
		// update the totals
		tot_lines += n_lines;
		tot_words += n_words;

		// then print the summary
		// the format is #lines\t#words\tfilename
		if(count_lines == COUNT_ON) {
			printf("%ld\t", n_lines);
		}
		if(count_words == COUNT_ON) {
			printf("%ld\t", n_words);
		}
		printf("%s\n", argv[i]);

		// the file has been read, so it can be closed
		fclose(ifile);
		// and the line is freed as well
		free(line);

		i++;
	}

	// if more than one file was counted, print the total
	if(i > optind + 1) {
		// the format is #lines\t#words\tfilename
		if(count_lines == COUNT_ON) {
			printf("%ld\t", tot_lines);
		}
		if(count_words == COUNT_ON) {
			printf("%ld\t", tot_words);
		}
		puts("total");
	}

	return EXIT_SUCCESS;
}

void print_usage(const char *program) {
	printf("Usage: %s [-l -w] [FILES] [...] [-h]\n", program);
}

// counts the words and lines in buf
// Returns:
// 0: buffer processed successfully
// 1: the buffer does not end in '\n', so it there may be a word in the remaining part
int process_buf(const char *buf, long int *nlines, long int *nwords) {
	long int words = 0;
	size_t wordlen = 0;
	size_t i = 0;
	while(buf[i] != '\0') {
		// if a blank is found two situations can arise:
		// the previous character was also a blank, in wich case just get the next character
		// the previous character(s) were non-blank => increase word count
		// NOTE: isspace matches (returns 0) on ' ', '\t', '\n', '\r', '\v'
		// subcase: if the character is '\n', increase line count
		if(buf[i] == '\n') {
			(*nlines)++;
		}
		if(isspace((unsigned char)buf[i]) != 0 && wordlen > 0) {
			wordlen = 0;
			words++;
		}
		// if a whitespace is not matched, increase word lenght
		else if(isspace((unsigned char)buf[i]) == 0){
			wordlen++;
		}
		i++;
	}

	// debug prints
	#ifdef DEBUG
	printf("#words in line = %ld\n", words);
	#endif

	*nwords += words;
	// the last char before '\0' was not a whitespace (word not finished in the current buffer)
	if(i > 0 && isspace((unsigned char)buf[i-1]) == 0) {
		return 1;
	}
	return 0;
}
