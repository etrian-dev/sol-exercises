// A copy of the cp command, using C library I/O calls (fread/fwrite...)
// Takes one input file and copies its into an output file, using a buffer
// of size specified as the third argument (in bytes, defaults to 256 bytes)

// NOTE: I/O done with libc calls are buffered by default

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
void cleanup(FILE *fp1, FILE *fp2, char *buf) {
	if(!fp1 || !fp2) {
		puts("Invalid file pointer");
	}
	else if(fclose(fp1) != 0 || fclose(fp2) != 0) {
		perror("Error closing file(s)");
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
		FILE *ifile = fopen(argv[1], "r");
		// if fopen failed, report the error and exit
		if(ifile == NULL) {
			perror(argv[1]);
			return EXIT_FAILURE;
		}
		
		// open the destinantion file in write mode (creates it if it doesn't exist)
		// NOTE: erases the content if it already exists
		FILE *destfile = fopen(argv[2], "w");
		if(destfile == NULL) {
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
		size_t read_ret;
		size_t write_ret;
		// loop until EOF is reached
		while(feof(ifile) == 0) {
			read_ret = fread(buf, sizeof(char), bufsz, ifile);
			// checks for errors thay may happen during the read
			if(read_ret < bufsz && ferror(ifile) != 0) {
				perror(argv[2]);
				exit(EXIT_FAILURE);
			}
			
			// write until the number of bytes read into the buffer by the previous call
			write_ret = fwrite(buf, sizeof(char), read_ret, destfile);
			// checks for errors thay may happen during the write
			if(read_ret < bufsz &&write_ret == -1) {
				perror(argv[2]);
				cleanup(ifile, destfile, buf);
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
		cleanup(ifile, destfile, buf);
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
