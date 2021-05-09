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
    pthread_mutex_init(&mux, NULL);

    unlink(ADDR);

    struct tpool *all_threads = malloc(sizeof(struct tpool));
    if(!all_threads) {
        DBG(printf("[SERVER %d]: Cannot create array of TIDs: %s\n", getpid(), strerror(errno)));
        return 1;
    }
    all_threads->pool = malloc(sizeof(pthread_t));
    if(!all_threads->pool) {
        DBG(printf("[SERVER %d]: Cannot create array of TIDs: %s\n", getpid(), strerror(errno)));
        return 1;
    }
    all_threads->pool[0] = pthread_self(); // this thread as well
    all_threads->poolsize = 1;
    // array to be filled with all thread ids except
    //the handler thread created below

    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

    // spawn the thread who handles all the termination signals
    pthread_t term_handler; // thread who handles the process termination signals
    if(pthread_create(&term_handler, &attrs, handle_termsig, (void*)all_threads) != 0) {
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
    if((server_sock = sock_init(ADDR, strlen(ADDR))) == -1) {
        DBG(printf("[SERVER %d]: Cannot create server socket: %s\n", getpid(), strerror(errno)));
        return 1;
    }

    // this thread accepts connections on the socket just created
    // for each accepted connection, a detached thread is spawned
    // Meanwhile, this thread keeps trying to accept other incoming connections
    pthread_mutex_lock(&mux);
    term = 0;
    pthread_mutex_unlock(&mux);
    while(1) {
        pthread_mutex_lock(&mux);
        if(term) {break;}
        pthread_mutex_unlock(&mux);

        int client_sock;
        if((client_sock = accept(server_sock, NULL, NULL)) == -1) {
            DBG(printf(
                "[ACCEPTOR THREAD %ld]: Connection requested to listening socket %d REFUSED: %s\n",
                pthread_self(), server_sock, strerror(errno)));
            pthread_mutex_lock(&mux);
            if(term) {break;}
            pthread_mutex_unlock(&mux);
        }

        // All went well: notify it if debug is on
        DBG(printf("[ACCEPTOR THREAD %ld]: Connection to client on socket %d ACCEPTED\n", pthread_self(), client_sock));

        // a thread is spawned to handle the connection on client_sock
        pthread_t worker;
        // the worker thread needs the socket used by the accepted client to read & write
        if(pthread_create(&worker, &attrs, work, (void*)client_sock) == -1) {
            DBG(printf("[ACCEPTOR THREAD %ld]: Cannot spawn worker thread: %s", pthread_self(), strerror(errno)));
        }

        // the newly created thread is added to the array of threads
        all_threads->pool = realloc(all_threads->pool, sizeof(pthread_t) * (all_threads->poolsize + 1));
        all_threads->pool[all_threads->poolsize] = worker;
        all_threads->poolsize++;

        // keep trying to accept connections...
    }
    pthread_attr_destroy(&attrs);

    return 0;
}
