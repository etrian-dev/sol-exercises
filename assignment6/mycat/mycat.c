// This program is a copy of the famous unix command cat
// The program expects a list of files (at least one) to be concatenated to stdout
// So the typical usage is ./mycat file1 [fileN]

// my includes
//std lib includes
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
// syscall includes
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void print_usage(const char *program);

// define a reasonable buffer size for reads
#define BUFSZ 1024

int main(int argc, char **argv) {
	if(argc <= 1) {
		printf("No arguments given\n");
		print_usage(argv[0]);
	}
	else {
		// define a fixed buffer to read files
		char buffer[BUFSZ];
		
		// for each file: open it, read it and write it to stdout, close it
		int i = 1;
		while(i < argc) {
			int ifile_fd = open(argv[i], O_RDONLY);
			// error trying to open this file
			if(ifile_fd == -1) {
				perror(argv[i]);
				// keep concatenating the others
			}
			else {
				ssize_t count_elem;
				
				do {
					// resets errno
					errno = 0;
					// reads at most BUFSZ bytes
					count_elem = read(ifile_fd, buffer, BUFSZ);
					if(count_elem == -1) {
						perror("read() error");
					}
					else {
						// write the number of bytes read to stdout (fd 1, opened by default)
						if(write(1, buffer, count_elem) == -1) {
							perror("write() error");
						}
					}
				} while(count_elem > 0);
				// count_elem == 0 (EOF) and errors are handled inside the loop
				
				// this fd can be closed
				if(close(ifile_fd) == -1) {
					perror("close() error");
					return EXIT_FAILURE;
				}
			}
			
			i++;
		}
	}
	return EXIT_SUCCESS;
}

void print_usage(const char *program) {
	printf("Usage: %s <file1> [fileN]\n", program);
}
