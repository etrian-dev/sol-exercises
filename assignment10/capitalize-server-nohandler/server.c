// server capitalizzazione multithreaded con gestione segnali tramite thread dedicato

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

int main(int argc, char **argv) {
    pthread_t *all_tids = malloc(sizeof(pthread_t));
    if(!all_tids) {
        DBG(printf("[SERVER %d]: Cannot create array of TIDs: %s\n", getpid(), strerror(errno)));
        return 1;
    }
    all_tids[0] = pthread_self(); // this thread as well
    // array to be filled with all thread ids except
    //the handler thread created below

    // spawn the thread who handles all the termination signals
    pthread_t term_handler; // thread who handles the process termination signals
    if(pthread_create(&term_handler, NULL, &handle_termsig, all_tids) == -1) {
        DBG(printf("[SERVER %d]: Cannot create termination handler thread. Aborting: %s\n", getpid(), strerror(errno)));
        return 1;
    }

    // block all signals
    sigset_t masked_signals;
    if(sigfillset(&masked_signals) == -1) {
        perror("Cannot fill the signal mask");
        exit(-1);
    }
    if(pthread_sigmask(SIG_BLOCK, &masked_signals, NULL) != 0) {
        perror("Error in blocking signals");
        exit(-1);
    }
    // then install the signal handler to ignore SIGPIPE
    ignore_pipe();

    // DO THINGS: all threads spawned from here and their children will inherit the signal mask
    // that is, all thread spawned by main (workers)
    // WILL HAVE ALL SIGNALS MASKED

    // create the socket and bind it to a known address
    int sock_fd;
    if((sock_fd = sock_init(ADDR, strlen(ADDR))) == -1) {
        DBG(printf("[SERVER %d]: Cannot create server socket: %s\n", getpid(), strerror(errno)));
        return 1;
    }

    // the socket is copied into a global volatile sig_atomic_t variable
    // be closed by the signal handler
    server_sock = (sig_atomic_t)sock_fd;

    // sets attribute detached on thread creation just to avoid an additional call
    pthread_attr_t attributes;
    if(pthread_attr_init(&attributes) != 0) {
        perror("Failed to initialize thread attr");
        exit(-1);
    }
    if(pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED) != 0) {
        perror("Failed to set spawned threads to DETACHED");
        exit(-1);
    }

    // this thread accepts connections on the socket just created
    // for each accepted connection, a detached thread is spawned
    // Meanwhile, this thread keeps trying to accept other incoming connections
    while(1) {
        int client_sock;
        if((client_sock = accept(sock_fd, NULL, NULL)) == -1) {
            DBG(printf("[ACCEPTOR THREAD %ld]: Connection requested to listening socket %d REFUSED: %s\n", pthread_self(), sock_fd, strerror(errno)));
        }

        // All went well: notify it if debug is on
        DBG(printf("[ACCEPTOR THREAD %ld]: Connection to client on socket %d ACCEPTED\n", pthread_self(), client_sock));

        // a thread is spawned to handle the connection on client_sock
        pthread_t worker;
        // the worker thread needs the socket used by the accepted client to read & write
        if(pthread_create(&worker, &attributes, work, (void*)client_sock) == -1) {
            DBG(printf("[ACCEPTOR THREAD %ld]: Cannot spawn worker thread: %s", pthread_self(), strerror(errno)));
        }
        // the thread is created as detatched, so that it can terminate indipendently of any thread
        // and its resources will be freed

        // keep trying to accept connections...
    }

    return 0;
}
