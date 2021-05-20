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
void install_sighandlers(void) {
    // mask all signals first, then install signal handlers
    sigset_t masked_signals, oldmask;

    if(sigfillset(&masked_signals) == -1) {
        perror("Cannot fill the signal mask");
        exit(-1);
    }
    // NOTE: the old mask is saved into oldmask to restore it afterwards
    if(pthread_sigmask(SIG_BLOCK, &masked_signals, &oldmask) != 0) {
        perror("Error in blocking signals");
        exit(-1);
    }

    // now install all signal handlers for the blocked signals
    struct sigaction handler;
    memset(&handler, 0, sizeof(handler)); // zeroed to be safe
    // SIGPIPE must be ignored
    handler.sa_handler = SIG_IGN; // ignores the signal
    if(sigaction(SIGPIPE, &handler, NULL) == -1) {
        perror("Cannot ignore SIGPIPE");
        exit(-1);
    }

    // set the signal mask bits corresponding to the termination signals
    // SIGINT, SIGTERM, SIGHUP for which the same signal handler will be installed
    if(sigemptyset(&masked_signals) == -1) {
        perror("Cannot empty the signal mask");
        exit(-1);
    }
    if(	sigaddset(&masked_signals, SIGINT) == -1
       	|| sigaddset(&masked_signals, SIGQUIT) == -1
        || sigaddset(&masked_signals, SIGTERM) == -1
        || sigaddset(&masked_signals, SIGHUP) == -1) {
        perror("Signal mask cannot be set properly");
        exit(-1);
    }

    memset(&handler, 0, sizeof(handler)); // zeroed to be safe
    handler.sa_handler = term_handler;
    handler.sa_mask = masked_signals; // the mask contains all the termination signals handled by this handler
    if(	sigaction(SIGINT, &handler, NULL) == -1
            || sigaction(SIGQUIT, &handler, NULL) == -1
            || sigaction(SIGTERM, &handler, NULL) == -1
            || sigaction(SIGHUP, &handler, NULL) == -1) {
        // if any of these handler registering calls failed, exit
        perror("Cannot register termination handler for some signals");
        exit(-1);
    }

    // now all signal handlers are registered, so restore the old set of masked signals
    if(pthread_sigmask(SIG_UNBLOCK, &masked_signals, NULL) != 0) {
        perror("Error in restoring the old blocked signals mask");
        exit(-1);
    }
}

// This is the termination signal(s) handler. The specific signal received is the parameter
void term_handler(int signal) {
    // Notifies on the terminal that the signal was received
    // TODO: Maybe it's not a good thing

    // signal to all threads to terminate through a global variable
    terminate = 1;

    // exit with the received signal as a code
}
