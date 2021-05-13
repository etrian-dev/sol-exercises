// my header
#include <util.h>
// posix threads header
#include <pthread.h>
// syscall headers
#include <unistd.h>
// std headers
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

// functions to handle signals

// this function is called by the server at startup and its job is to
// register the signal handler specified below for all termination signals
// and make the server ignore SIGPIPE
void ignore_pipe(void) {
    // ignore SIGPIPE
    struct sigaction ignore_pipe;
    memset(&ignore_pipe, 0, sizeof(ignore_pipe)); // zeroed to be safe
    // SIGPIPE must be ignored
    ignore_pipe.sa_handler = SIG_IGN; // ignores the signal
    if(sigaction(SIGPIPE, &ignore_pipe, NULL) == -1) {
        perror("Cannot ignore SIGPIPE");
        exit(-1);
    }
}
