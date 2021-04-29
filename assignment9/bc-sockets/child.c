#include <sys/types.h>
#include <unistd.h>
// std headers
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>


// code executed by the child process, executing bc
void child(int *in_pipe, int *out_pipe) {
	// connect pipes to receive input from the parent and send output to it
	if(dup2(in_pipe[0], 0) == -1) {
		perror("Cannot dup pipe fd");
		exit(-1);
	}
	if(dup2(out_pipe[1], 1) == -1) {
		perror("Cannot dup pipe fd");
		exit(-1);
	}
	// dup child's stderr to be redirected trough the pipe as well
	if(dup2(out_pipe[1], 2) == -1) {
		perror("Cannot dup pipe fd");
		exit(-1);
	}
	// then close all pipe fds so that only stdin and stdout are open in this process
	if( close(in_pipe[0]) == -1
		|| close(in_pipe[1]) == -1
		|| close(out_pipe[0]) == -1
		|| close(out_pipe[1]) == -1)
	{
		perror("Some fd cannot be closed");
		exit(-1);
	}

	// options -lq are needed to have the math library and no welcome message
	execl("/usr/bin/bc", "/usr/bin/bc", "-lq", NULL);
	// exec returns only if it fails
	perror("Cannot exec bc(1)");
	exit(-1);
}
