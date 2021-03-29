// This program sleeps X seconds, then prints the child_pid process id and parent process id
// X is the argument passed to the program

// std includes
#include <stdio.h>
#include <string.h>
#include <errno.h>
// process-related calls includes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
// declarations not part of c99
long int strtol(const char*, char**, int);
// uses /bin/sleep to sleep X seconds
#define SLEEP_PROG "/bin/sleep"
// utility function
int isNumber(const char* s, long* n);

int main(int argc, char **argv) {
	// fork the parent process
	int child_pid;
	if((child_pid = fork()) == -1) {
		perror("Cannot fork() process");
		return 1;
	}
	
	// the child sleeps for argv[1] seconds (converted to an integer), then exits
	if(child_pid == 0) {
		long nsleep;
		// convert argv[1] to an integer & check its value
		if(isNumber(argv[1], &nsleep) == 0 && nsleep >= 0) {
			// sleeps argv[1] seconds, using /bin/sleep
			execl(SLEEP_PROG, SLEEP_PROG, argv[1], (char*)NULL);
			// if exec returned, it failed for sure
			perror("Cannot exec /bin/sleep");
			return 2;
		}
	}
	// the parent waits for its child's termination and prints pid, child_pid
	else {
		// I'm not interested in the return status, so NULL is passed in as the second arg
		if(waitpid(child_pid, NULL, 0) == -1) {
			perror("waiting for child_pid failed");
		}
		else {
			printf("Parent PID: %d\nChild PID: %d\n", getpid(), child_pid);
		}
	}
	// then the parent terminates with status 0 (success)
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
