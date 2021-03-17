// This program searches a file recursively from the path given as the first argument
// and displays for each match the absolute path and the last modified date
// (as a human-readable date)

// my includes
#include <utilities.h>
#include <explore-dir.h>
// standard lib includes
#include <stdio.h>
#include <stdlib.h>

// Declared here just because it's not defined in C99, so gcc would give a warning
char *strndup(const char *, size_t);

// usage message, printed if the wrong number of aguments is given to the program
void print_usage(const char *fname) {
	printf("Usage: %s <dir> <fname>\n", fname);
}

// Main function, does what it's stated above
// The path is argv[1] and the filename is argv[2]
int main(int argc, char **argv) {
	if(argc != 3) {
		print_usage(argv[0]);
	}
	else {
		#ifdef DEBUG
		// Print what the arguments were
		printf("Searching for files having name\"%s\" in directory \"%s\"\n", argv[2], argv[1]);
		#endif
		
		char *abspath = argv[1];
		int to_free = 0;
		// if argv[1] is not an absolute path, then obtain it
		if(argv[1][0] != '/') {
			to_free = 1;
			abspath = get_abspath(argv[1]);
		}
		
		// explore the directory tree rooted at abspath for files matching argv[2]
		int status = explore_dir(abspath, argv[2]);
		// print the appropriate error message it some error occurred
		err_report(status, argv[1]);
		
		if(to_free == 1) {
			free(abspath);
		}
	}
	return EXIT_SUCCESS;
}
