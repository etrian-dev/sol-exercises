// Spawns N processes in total, where N is the parameter to the program
// Each child process should print its pid, then fork and wait for the termination
// of its child process, until n processes are terminated.
// Dashes in front of the output indicate the depth in the process tree, starting from
// N (the parent of all processes) up to 0 (the last process does not spawn anything) 
// The sample output is:
/*
./family 4
---- 20894: creo un processo figlio
--- 20895: creo un processo figlio
-- 20896: creo un processo figlio
- 20897: creo un processo figlio
20898: sono l'ultimo discendente
20898: terminato con successo
- 20897: terminato con successo
-- 20896: terminato con successo
--- 20895: terminato con successo
---- 20894: terminato con successo
*/

// std includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
// process-related calls includes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
// c99 does not contain it, so to avoid warnings I declare it here
long strtol(const char*, char**, int);

void print_usage(const char *prog) {printf("Usage: %s <n(>1)>\n", prog);}

int isNumber(const char* s, long* n);

int main(int argc, char **argv) {
	if(argc != 2) {
		// Supplied 0 or >2 args to the program
		print_usage(argv[0]);
		return 0;
	}
	// Convert the argument to the program (argv[1]) into an integer
	long n;
	int res = isNumber(argv[1], &n);
	// if isNumber returned a non-zero value or n <= 0, then print the usage message
	if(res != 0 || n < 0) {
		print_usage(argv[0]);
		return 0;
	}
	// if n is at least 1, then it's not the last in the hierarchy
	// print its pid, spawn a child process and wait for its termination, then exit
	if(n > 0) {
		// n dashes must be printed before the pid
		char *dashes = malloc((n + 1) * sizeof(char));
		if(!dashes) {
			perror("malloc dashes[]");
			exit(-1);
		}
		memset(dashes, (int)'-', n * sizeof(char));
		dashes[n] = '\0'; // never forget the terminator
		
		// prints pid and tries to flush stdout
		printf("%s %d: creo un processo figlio\n", dashes, getpid());
		if(fflush(stdout) == EOF) {
			perror("Failed to flush stdout");
			// non-critical failure, but output will be messed up
		}
		
		// spawn child process
		int child;
		if((child = fork()) == -1) {
			perror("Cannot fork()");
			exit(-1);
		}
		
		// The child executes this program, but n is decreased by 1
		if(child == 0) {
			// using snprintf to convert an integer (n-1) to a string
			char *arg = calloc((strlen(argv[1]) + 1), sizeof(char));
			snprintf(arg, strlen(argv[1]) + 1, "%ld", n - 1);
			execl(argv[0], argv[0], arg, NULL);
			
			perror("Cannot run execl");
			free(arg);
			exit(1);
		}
		
		// The parent process waits for its child's termination
		int status;
		if((child = waitpid(child, &status, 0)) == -1) {
			perror("error waiting for the child to terminate");
			exit(-1);
		}
		// determine if the child exited correctly
		if(WIFEXITED(status)) {
			printf("%s %d: terminato con successo\n", dashes, getpid());
		}
		// frees memory for the dashes string
		free(dashes);
		// exit flushes stdout
		exit(0);
	}
	// Special case: the last descendant prints its own message
	else {
		printf("%d: sono l'ultimo discendente\n", getpid());
		printf("%d: terminato con successo\n", getpid());
		exit(0);
	}
	
	return 0;
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

