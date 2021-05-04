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
    struct sigaction ignore_pipe;
    memset(&ignore_pipe, 0, sizeof(ignore_pipe)); // zeroed to be safe
    // SIGPIPE must be ignored
    ignore_pipe.sa_handler = SIG_IGN; // ignores the signal
    if(sigaction(SIGPIPE, &ignore_pipe, NULL) == -1) {
        perror("Cannot ignore SIGPIPE");
        exit(-1);
    }

    // set the signal mask to mask only to the termination signals, for which one
    // signal handler will be installed
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
    struct sigaction handler;
    memset(&handler, 0, sizeof(handler)); // zeroed to be safe
    // other termination signals are set to one signal handler (term_handler)
    // so all other termination signals specified in the mask should be masked
    // during that signal handler
    handler.sa_handler = term_handler;
    handler.sa_mask = masked_signals; // the mask contains only the termination signals handled by this handler
    if(	sigaction(SIGINT, &handler, NULL) == -1
            || sigaction(SIGQUIT, &handler, NULL) == -1
            || sigaction(SIGTERM, &handler, NULL) == -1
            || sigaction(SIGHUP, &handler, NULL) == -1) {
        // if any of these handler registering calls failed, exit
        perror("Cannot register termination handler for some signals");
        exit(-1);
    }

    // now all signal handlers are registered, so restore the old set of masked signals
    if(pthread_sigmask(SIG_SETMASK, &oldmask, NULL) != 0) {
        perror("Error in restoring the old blocked signals mask");
        exit(-1);
    }
}

// signal holds the received signal
void term_handler(int signal) {
    // Notifies on the terminal the termination signal
    // TODO: Maybe it's not a good thing
    write(1, "Received signal\n", 17);
    // this handler will unlink the socket and close it
    if(unlink(ADDR) == -1) {
        write(1, "Cannot unlink socket", 21);
        _exit(-1);
    }
    // the socket is stored in this global variable defined in util.h
    if(close((int)server_sock) == -1) {
        write(1, "Server socket cannot be closed\n", 50);
        _exit(-1);
    }

    // esco col numero di segnale ricevuto
    _exit(signal);
}
