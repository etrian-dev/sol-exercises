// header with useful information shared by the server and the clients

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stddef.h>
#include <signal.h> // for the definition of the "sig_atomic_t" type

// macro to wipe out debug prints from release executables
#if defined(DEBUG)
#define DBG(X) X
#else
#define DBG(X)
#endif
// default buffers size
#define BUFSZ 1000

// known path to the socket file
#define ADDR "sock"
// maximum socket path lenght
#define PATHLEN_MAX 108
// function used to initialize a socket for the server
int sock_init(char *address, size_t len);

//	isNumber tenta di convertire una stringa s in un long int. Ritorna
//	0: ok
//	1: non e' un numero
//	2: overflow/underflow
int isNumber(const char* s, long* n);

// global variables used to pass things indirectly to the signal handler
volatile sig_atomic_t server_sock;

// Function that install the signal handler to ignore SIGPIPE (process-wide)
void ignore_pipe(void);

// This structure holds all thread IDs in the process, except the handler's TID
struct tpool {
    pthread_t *pool;
    size_t number;
};

// This function is executed by the handler thread and handles termination signals
// SIGINT and SIGQUIT to ensure the server exits properly
void *handle_termsig(void *all_threads);

// This is the function executed by worker threads
// Its parameter is the queue where the manager inserts the socket file descriptor from the
// accepted connections
void *work(void *clients_q);

// mutex to ensure ME access to the queue
extern pthread_mutex_t mux;
// cond variable to signal new elements in the queue
extern pthread_cond_t new_client;

// This is the queue's node: it holds a pointer to the data (the data itself in this case)
struct node_t {
    void *data_ptr;
    struct node_t *next;
};
// The Queue struct is just a wrapper to pass head and tail pointers toghether
struct Queue {
    struct node_t *head;
    struct node_t *tail;
};

// This operation (in ME) adds at the tail of the queue
// the element containing data of the specified size
// Returns 0 if it succeeds, -1 otherwise
int enqueue(struct Queue **head, struct Queue **tail, const void *data, size_t size);
// This operation (in ME) returns the head element in the queue, and removes it from it
// If the queue is empty or some error occurred, returns NULL.
// The node_t* returned must be then freed by the caller
struct node_t *pop(struct Queue **head, struct Queue **tail);

#endif
