// my header
#include <util.h>
// syscall header
#include <unistd.h>
// std headers
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

// code executed by the child process: 
// connects stdin and and stdout to the parent process trough pipes, then exec bc(1)
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
	// redirect child's stderr as well
	if(dup2(out_pipe[1], 2) == -1) {
		perror("Cannot dup pipe fd");
		exit(-1);
	}
	// then close all pipes'fds so that just stdin, stdout and stderr are open when
	// bc is executed
	if( close(in_pipe[0]) == -1
		|| close(in_pipe[1]) == -1
		|| close(out_pipe[0]) == -1
		|| close(out_pipe[1]) == -1)
	{
		perror("Some pipe fd cannot be closed");
		exit(-1);
	}
    
    // exec bc: reads expressions from stdin and outputs the result on stdout
    // and any error on stderr
	// options -lq are needed to have the math library and no welcome message
	execl(BC, BC, "-lq", NULL);
    
	// exec returns only if it fails
	perror("Cannot exec bc");
	exit(-1);
}
