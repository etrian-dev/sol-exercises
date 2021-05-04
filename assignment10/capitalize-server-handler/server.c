// server capitalizzazione multithreaded con gestione segnali tramite signal handlers

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
    // installs signal handlers
    install_sighandlers();

    // DO THINGS

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
