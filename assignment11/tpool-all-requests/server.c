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
#include <limits.h>

#define TPOOLSZ_DFL 10

// Read the option -N (thread pool size)
// If no such option is given, set the threadpool size is set to TPOOLSZ_DFL by default
// It returs the size of the threadpool
long int parse_tpoolsz(int nargs, char **args);
// Initializes the shared TIDs structure to contain n + 1 threads and puts the manager in position 0
// So it must be called from the manager BEFORE spawning worker threads
int init_tinfo(struct tpool *all_threads);

pthread_cond_t new_client = PTHREAD_COND_INITIALIZER;

int main(int argc, char **argv) {
    // parsing dell'opzione -N per ottenere la size della threadpool
    long int n = parse_tpoolsz(argc, argv);

    struct tpool thread_info;
    if(init_tinfo(&thread_info) == -1) {
        // something in the initialization failed
        return 1;
    }

    // Queue structure to communicate
    struct Queue *Q = malloc(sizeof(struct Queue));
    if(!Q) {
    Q->head = NULL;
    Q->tail = NULL;

    if(pthread_detach(pthread_self()) != 0) {
        DBG(printf("[SERVER %d]: Cannot detach the main thread: %s\n", getpid(), strerror(errno)));
        return 1;
    }

    // Spawn all worker threads
    long i;
    for(i = 0; i < n; i++) {
        // The queue struct is passed as a parameter to the worker; It pops from it
        if(pthread_create(&all_threads->pool[i+1], NULL, work, Q) == -1) {
            DBG(printf("[SERVER %d]: Cannot spawn worker thread %ld: %s\n", getpid(), i, strerror(errno)));
            return 1;
        }
    }

    // Set attributes to create a detached thread (the handler thread)
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

    // spawn the thread who handles all the termination signals
    // created as detached because it's not joined by any other thread
    pthread_t term_handler;
    if(pthread_create(&term_handler, &attrs, handle_termsig, (void*)all_threads) != 0) {
        DBG(printf("[SERVER %d]: Cannot create termination handler thread: %s\n", getpid(), strerror(errno)));
        return 1;
    }
    // destroy attrs
    pthread_attr_destroy(&attrs);

    // block all signals sent to this thread (inherited by all threads spawned by this one)
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

    // Set up a socket to listen for connections

    // create the socket and bind it to a known address
    if((server_sock = sock_init(ADDR, strlen(ADDR))) == -1) {
        DBG(printf("[SERVER %d]: Cannot create server socket: %s\n", getpid(), strerror(errno)));
        return 1;
    }

    // this thread accepts connections on the socket just created
    // for each accepted connection, a message is put into the queue for that client
    // to be served by a worker thread
    // Meanwhile, this thread keeps trying to accept other incoming connections
    while(1) {
        int client_sock;
        if((client_sock = accept(server_sock, NULL, NULL)) == -1) {
            DBG(printf(
                "[ACCEPTOR THREAD %ld]: Connection requested to listening socket %d REFUSED: %d\n",
                pthread_self(), server_sock, errno));
            break;
        }

        // All went well: notify it if debug is on
        DBG(printf("[ACCEPTOR THREAD %ld]: Connection to client on socket %d ACCEPTED\n", pthread_self(), client_sock));

        // insert on the queue the new waiting client's socket fd
        if(enqueue(&(Q->head), &(Q->tail), (void*)client_sock, 4) == -1) {
            DBG(printf("[ACCEPTOR THREAD %ld]: Queue insert of connection %d FAILED\n", pthread_self(), client_sock));
            exit(-1);
        }
        // insert ok => signal a new element in the queue
        pthread_cond_signal(&new_client);

        // keep trying to accept connections...
    }

    return 0;
}

// Read the option -N (thread pool size)
// If no such option is given, set the threadpool size is set to TPOOLSZ_DFL by default
long int parse_tpoolsz(int nargs, char **args) {
    long int n = TPOOLSZ_DFL;
    errno = 0;
    if(nargs == 3 && strncmp(args[1], "-N", strlen(args[1]) + 1) == 0) {
        if(isNumber(args[2], &n) != 0 && n > 0) {
            DBG(printf("[SERVER %d]: Dimensione threadpool non valida: fallback %ld\n", getpid(), n));
            n = TPOOLSZ_DFL;
        }
    }
    else {
        DBG(printf("[SERVER %d]: Dimensione threadpool non valida: fallback %ld\n", getpid(), n));
        n = TPOOLSZ_DFL;
    }
    return n;
}

int init_tinfo(struct tpool *all_threads) {
    // this shared structure holds all the TIDs of the threads spawned, including the main thread
    // and exluding the thread that handles termination.
    all_threads = malloc(sizeof(struct tpool));
    if(!all_threads) {
        DBG(printf("[SERVER %d]: Cannot create TIDs info structure: %s\n", getpid(), strerror(errno)));
        return -1;
    }
    // The thread pool is formed by n worker threads, but the master must be counted in as well
    all_threads->pool = malloc((n + 1) * sizeof(pthread_t));
    if(!all_threads->pool) {
        DBG(printf("[SERVER %d]: Cannot create thread pool: %s\n", getpid(), strerror(errno)));
        return -1;
    }
    all_threads->pool[0] = pthread_self(); // the main thread is always the first in the array
    all_threads->number = n + 1;

    return 0;
}
