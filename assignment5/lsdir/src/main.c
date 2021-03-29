// This program takes a directory as argument and lists the contents of the corresponding
// directory tree (such as ls)

// my includes
#include <utilities.h>
// standard lib includes
#include <stdio.h>
#include <stdlib.h>

// Declared here just because it's not defined in C99, so gcc would give a warning
char *strndup(const char *, size_t);

// usage message, printed if the wrong number of aguments is given to the program
void print_usage(const char *fname) {
	printf("Usage: %s <dir>\n", fname);
}

// Main function, does what it's stated above
// The path is argv[1]
int main(int argc, char **argv) {
	if(argc != 2) {
		print_usage(argv[0]);
	}
	else {
		#ifdef DEBUG
		// Print what the arguments were
		printf("%s %s\n", argv[0], argv[1]);
		#endif
		
		// explore the directory tree rooted at abspath for files matching argv[2]
		int status = lsdir(argv[1]);
		// print the appropriate error message it some error occurred
		err_report(status, argv[1]);
	}
	return EXIT_SUCCESS;
}
