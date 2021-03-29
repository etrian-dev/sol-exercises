// This program takes an integer N as argument and creates N zombie processes
// (child processes whose parent terminated without waiting them: they'll eventually
// be terminated by the OS
// Visualize them with the following command: ps -A -ostat,pid,ppid | grep Z

// std includes
#include <stdio.h>
#include <string.h>
#include <errno.h>
// process-related calls includes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
// c99 does not contain it, so to avoid warnings I declare it here
long strtol(const char*, char**, int);

void print_usage(const char *prog) {printf("Usage: %s <n(>0)>\n", prog);}

int isNumber(const char* s, long* n);

#define MSG "Failed to create child process"

int main(int argc, char **argv) {
	long num;
	int ret = isNumber(argv[1], &num);
	if(argc < 1 || ret != 0 || num <= 0) {
		print_usage(argv[0]);
	}
	else {
		// spawn num processes and then sleep
		long i;
		for(i = 0; i < num; i++) {
			int pid_i = fork();
			if(pid_i == -1) {
				perror(MSG);
			}
			else if(pid_i == 0) {
				// the child process just terminates successfully
				__exit(0);
			}
		}
		// The parent process waits 10 seconds using /bin/sleep
		execl("/bin/sleep", "/bin/sleep", "10", (char*)NULL);
		perror("execl");
		return 1;
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
