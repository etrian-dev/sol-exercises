// A copy of the cp command, using UNIX system calls (open, read, write)
// Takes one input file and copies its into an output file, using a buffer
// of size specified as the third argument (in bytes, defaults to 256 bytes)

// NOTE: system calls do unbuffered I/O, thus the performance may be slowed down
// if many reads/writes need to be performed

// syscalls headers
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void *memset(const void*, int, size_t);
size_t strlen(const char*);

#define DEFAULT_BUFSZ 256 // default buffer dimension (no of bytes)

void print_usage(const char *prog) {
	printf("Usage: %s <scr> <dest> [bufsz]\n", prog);
}
// returns 0 on success, 1 if it cannot be converted, 2 if overflow would occurr
int isNumber(const char* s, long* n);
// performs cleanup of the opened files & alloc'd memory
void cleanup(int fd1, int fd2, char *buf) {
	if(close(fd1) == -1 || close(fd2) == -1) {
		perror("Serious trouble happened :(");
		exit(EXIT_FAILURE);
	}
	if(buf) {
		free(buf);
	}
}

int main(int argc, char **argv) {
	// the program expects one source file and one destination file
	// the buffer dimension is optional
	if(argc < 3 || argc > 4) {
		print_usage(argv[0]);
	}
	else {
		// open the source file in read mode
		int input_fd = open(argv[1], O_RDONLY);
		// if open failed, report the error and exit
		if(input_fd == -1) {
			perror(argv[1]);
			return EXIT_FAILURE;
		}
		
		// open the destinantion file in write mode (creates it if it doesn't exist)
		// NOTE: erases the content if it already exists
		#define PERMS S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH
		
		int dest_fd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, PERMS);
		if(dest_fd == -1) {
			perror(argv[2]);
			return EXIT_FAILURE;
		}
		
		// Choose the buffer of the dimension (or fall back to the default)
		long int bufsz = DEFAULT_BUFSZ;
		if(argv[3] != NULL) {
			if(isNumber(argv[3], &bufsz) != 0 || bufsz <= 0) {
				bufsz = DEFAULT_BUFSZ;
				
				#ifdef DEBUG
				puts("Sorry, buffer size should be greater than 0");
				#endif
			}
		}
		
		char *buf = calloc(bufsz, sizeof(char));
		if(buf == NULL) {
			perror("Allocation of buffer in __FILE__:__LINE__");
			exit(EXIT_FAILURE);
		}
		
		// copy the source file to the destination file
		ssize_t read_ret;
		ssize_t write_ret;
		while((read_ret = read(input_fd, buf, bufsz * sizeof(char))) > 0) {
			// write until the number of bytes read into the buffer by the previous call
			write_ret = write(dest_fd, buf, read_ret);
			// Error checking: if the write failed, then does cleanup and exits
			// NOTE: cleanup may exit if it fails to close the files
			if(write_ret == -1) {
				perror(argv[2]);
				cleanup(input_fd, dest_fd, buf);
				exit(EXIT_FAILURE);
			}
			
			#ifdef DEBUG
			printf("%lu bytes read from %s\n%lu bytes wrote from %s\n", read_ret, argv[2], write_ret, argv[3]);
			#endif
			
			// the buffer is resetted for before the next read
			buf = memset(buf, 0, bufsz * sizeof(char));
		}
		if(read_ret == -1) {
			perror(argv[2]);
			exit(EXIT_FAILURE);
		}
		
		// the file has been copied: do cleanup and then exit gracefully
		cleanup(input_fd, dest_fd, buf);
	}
	
	return EXIT_SUCCESS;
}

//	isNumber ritorna
//	0: ok
//	1: non e' un numbero
//	2: overflow/underflow
//
int isNumber(const char* s, long* n) {
  if (s==NULL) return 1;
  if (strlen(s)==0) return 1;
  char* e = NULL;
  errno=0;
  long val = strtol(s, &e, 10);
  if (errno == ERANGE) return 2;    // overflow
  if (e != NULL && *e == (char)0) {
    *n = val;
    return 0;   // successo 
  }
  return 1;   // non e' un numero
}
