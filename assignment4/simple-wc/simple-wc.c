// Simple wc clone (man 1 wc), limited to the -w and -l options
// counting respectively words and lines. If no options are specified, counts both
// The counts are on a per-file basis, but a total count is added if more than
// one file is supplied as an argument to the program (either if it exists or not)
// NOTE: Does not work properly on binary files because it uses fgets

// commandline options parsing header
#include <parse-opt.h> // needs option -I when compiling
// other std lib headers used
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> // for isspace & co.

// Defines the buffer size: the buffer is alloc's once and used
// to read lines (or at most BUFSZ characters from the input file)
#define BUFSZ 1000

// Function to process the buffer pointed to by buf
// Updates the counts of words and lines (passed by pointer)
int process_buf(const char *buf, long int *nlines, long int *nwords);

// Main function: receives the files to process as aruments and prints to stdout the results
int main(int argc, char **argv) {
	// If no args are provided, quit and display the usage string
	if(argc == 1) {
		print_usage(argv[0]);
		return EXIT_SUCCESS;
	}

	// Parse options (-w and -l supported); the flags declared below store whether
	// line or word count should be displayed in the output
	int count_words;
	int count_lines;
	if(parse_opt(argc, argv, &count_words, &count_lines) != 0) {
		puts("Failed to parse options");
		exit(EXIT_FAILURE);
	}

	// now open the file(s) and read them line by line
	// word and line count are always calculated on any file
	long int n_words = 0;
	long int n_lines = 0;
	long int tot_words = 0;
	long int tot_lines = 0;

	// parse_opt() (using getopt internally) permutes elements of argv so that
	// The last indices contain all the specified filenames
	// optind stores the first non-option argument to the program,
	// so argv must be scanned starting from this index
	int i = optind;
	while(i < argc) { // argc - 1 is always the last arg given to the program
		#ifdef DEBUG // prints the filename being processed
		printf("Processing %s...\n", argv[i]);
		#endif

		FILE *ifile = fopen(argv[i], "r");
		// If (for some reason) a file can't be opened,
		// keep running on the remaining ones as wc does
		if(!ifile) {
			// notify the fopen error on stdout
			perror(argv[i]);
		}

		// read the file line by line and update the counts
		n_words = 0;
		n_lines = 0;
		int word_longer_than_BUFSZ = 0; // this should be self-explanatory, but...
		// when counting words a line/word might span across different reads
		// and thus this is needed to keep track of words unfinished in the previous buffer

		char *line = calloc(BUFSZ, sizeof(char));
		if(!line) {
			perror("Calloc error. (__FILE__:__LINE__)");
			exit(EXIT_FAILURE);
		}

		// while EOF is not reached, read up to BUFSZ characters and process that buffer
		while(feof(ifile) == 0) {
			char *s = fgets(line, BUFSZ, ifile);
			if(!s && feof(ifile) == 0) {
				perror("Error reading data from file. (__FILE__:__LINE__)");
				exit(EXIT_FAILURE);
			}
			// 1 -> word/line not ended by this buffer
			// 0 -> buffer finishes with a '\n' (last non-NULL character)
			int return_code = process_buf(line, &n_lines, &n_words);
			// if the previous buffer(s) did not end the word and this did,
			// then add one word to the count
			if(word_longer_than_BUFSZ == 1 && return_code == 0) {
				n_words++;
			}
			// this buffer didn't end the line, so turn the flag ON
			else if(word_longer_than_BUFSZ == 0 && return_code == 1) {
				word_longer_than_BUFSZ = 1;
			}

			#ifdef DEBUG // print a summary up to this iteration on the file
			printf("#lines = %ld\n#words = %ld\n", n_lines, n_words);
			#endif

			// reset the line to zeros
			line = memset(line, 0, BUFSZ * sizeof(char));
		}
		// file processed: update the totals and print a summary of the file
		tot_lines += n_lines;
		tot_words += n_words;


		// print a summary of the counts in the files (ignored if that option is turned OFF)
		// the format is #lines\t#words\tfilename
		if(count_lines == COUNT_ON) {
			printf("%ld\t", n_lines);
		}
		if(count_words == COUNT_ON) {
			printf("%ld\t", n_words);
		}
		printf("%s\n", argv[i]);

		#ifdef DEBUG // print an annotated summary
		printf("#lines in %s: %ld\n#words in %s: %ld\n", tot_lines, tot_words);
		#endif

		// close the file & free the buffer
		fclose(ifile);
		free(line);

		// then get the next arg in argv (next filename)
		i++;
	}

	// if more than one file was given, print the total summary
	if(i > optind + 1) {
		// the format is #lines\t#words\tfilename
		if(count_lines == COUNT_ON) {
			printf("%ld\t", tot_lines);
		}
		if(count_words == COUNT_ON) {
			printf("%ld\t", tot_words);
		}
		puts("total"); // because wc says so
	}

	return EXIT_SUCCESS;
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
