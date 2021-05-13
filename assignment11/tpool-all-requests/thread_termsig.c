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
void *handle_termsig(void *all_threads) {
    // defines the set of termination signals first
    sigset_t termsigs;
    if(sigemptyset(&termsigs) == -1) {
	DBG(printf("[TERM HANDLER %ld]: Cannot set the termination mask: %s\n", pthread_self(), strerror(errno)));
	exit(-1);
    }
    if(	sigaddset(&termsigs, SIGINT) == -1
	|| sigaddset(&termsigs, SIGQUIT) == -1)
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

    // join all other threads (except main, because it hangs)
    size_t i;
    struct tpool *threadpool = (struct tpool*)all_threads;
    for(i = threadpool->poolsize - 1; i > 0 ; i--) {
	if(pthread_join(threadpool->pool[i], NULL) != 0) {
	    DBG(printf("[TERM HANDLER %ld]: Cannot join thread %ld: %s\n", pthread_self(), threadpool->pool[i], strerror(errno)));
	}
    }

    pthread_t main = threadpool->pool[0];

    free(threadpool->pool);
    free(threadpool);

    pthread_kill(main, SIGKILL);

    // not actually executed
    return (void*)signal;
}
