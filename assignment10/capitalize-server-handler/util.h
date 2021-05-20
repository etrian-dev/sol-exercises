// header with useful information shared by the server and the clients

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stddef.h>
#include <unistd.h> // for ssize_t
#include <signal.h> // for the definition of the "sig_atomic_t" type

// macro to wipe out debug prints from release executables
#if defined(DEBUG)
#define DBG(X) X
#else
#define DBG(X)
#endif

/* Read "n" bytes from a descriptor */
ssize_t readn(int fd, void *ptr, size_t n);
/* Write "n" bytes to a descriptor */
ssize_t writen(int fd, void *ptr, size_t n);

// global flag to signal to detached threads to terminate
volatile sig_atomic_t terminate;

// functions related to signal handlers
void install_sighandlers(void);
void ingnore_pipe(void);
void term_handler(int signal);

// known path to create the socket file
#define ADDR "sock"

// maximum socket path lenght
#define PATHLEN_MAX 108

#define BUFSZ 1000

// function used to initialize a socket for the server
int sock_init(char *address, size_t len);

// this is the function executed by worker threads spawned by this thread
void *work(void *client_sock);

#endif
