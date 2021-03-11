#include <parse-opt.h>
// std lib headers
#include <unistd.h> // for getopt

// I want to avoid all of stdio
int printf(const char *format, ...);

// prints the help message to stdout
void print_usage(const char *program) {
	printf("Usage: %s [-l -w] [FILES] [...] [-h]\n", program);
}

int parse_opt(int argc, char **argv, int *count_words, int *count_lines) {
	// initializes the count flags to OFF
	*count_words = COUNT_OFF;
	*count_lines = COUNT_OFF;
	// search for the help option (-h) first
	opterr = 0; // error prints off now
	int option;
	while((option = getopt(argc, argv, "h")) != -1) {
		if(option == 'h') {
			// help option recognized: print usage message
			print_usage(argv[0]);
			return 0;
		}
	}
	
	// otherwise search for the options in argv (permutes them so that unrecognized ones are left at the end)
	opterr = 1; // prints on stderr on
	optind = 1; // restart the search from the beginning of the arglist
	while((option = getopt(argc, argv, OPSTRING)) != -1) {
		switch(option) {
		case 'w': // recognized the -w option: the program will count words
			*count_words = COUNT_ON;
			break;
		case 'l': // recognized the -w option: the program will count lines
			*count_lines = COUNT_ON;
		/*
		Error handling?
		*/
		}
	}
	// print both line and word count if no option is specified
	if(count_words == COUNT_OFF && count_lines == COUNT_OFF) {
		*count_lines = *count_words = COUNT_ON;
	}
	
	// success
	return 0;
}
