// code executed by the thread who handles all termination signals sent to the server process

// my header
#include <util.h>
// posix threads header
#include <pthread.h>
// syscall headers
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
// std headers
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

// this thread function handles all termination signals
// to do so, it knows all other TIDs so that they can be killed before exiting
void *handle_termsig(void *all_tids) {
    // defines the set of termination signals first
    sigset_t termsigs;
    if(sigemptyset(&termsigs) == -1) {
	DBG(printf("[TERM HANDLER %ld]: Cannot set the termination mask: %s\n", pthread_self(), strerror(errno)));
	exit(-1);
    }
    if(	sigaddset(&termsigs, SIGINT) == -1
	|| sigaddset(&termisigs, SIGTERM == -1
	|| sigaddset(&termsigs, SIGQUIT) == -1
	|| sigaddset(&termsigs, SIGHUP) == -1)
    {
	DBG(printf("[TERM HANDLER %ld]: Cannot set the termination mask: %s\n", pthread_self(), strerror(errno)));
	exit(-1);
    }
    // mask prepared: wait for them
    int signal;
    if(sigwait(&termsigs, &signal) != 0) {
	DBG(printf("[TERM HANDLER %ld]: Bad mask to sigwait: %s\n", pthread_self(), strerror(errno)));
	exit(-1);
    }
    // in this case no matter what the received signal is, close sockets and dealloc resources
    // Notifies on the terminal the termination signal
    // TODO: Maybe it's not a good thing
    printf("Received signal %d. Terminating...\n", signal);
    // unlink the socket and close it
    // the socket is stored in this global variable defined in util.h
    if(close((int)server_sock) == -1) {
	DBG(printf("[TERM HANDLER %ld]: Server socket cannot be closed: %s\n", pthread_self(), strerror(errno)));
	exit(-1);
    }
    if(unlink(ADDR) == -1) {
	DBG(printf("[TERM HANDLER %ld]: Cannot unlink the socket file: %s\n", pthread_self(), strerror(errno)));
	exit(-1);
    }
    // free the workers'buffers

    // exit with the received signal number
    exit(signal);

    // not actually executed
    return (void*)0;
}
