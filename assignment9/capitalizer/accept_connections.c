#include <pthread.h>
// sockets headers
#include <sys/un.h>
#include <sys/socket.h>
// I/O calls
#include <stdio.h>
#include <errno.h>
#include <string.h>

// macro to wipe out debug prints from release executables
#if defined(DEBUG)
#define DBG(X) X
#else
#define DBG(X)
#endif

void *work(void *client_sock);

// the socket where the thread should accept connections is the one passed as a parameter
// despite the cast it's an integer
void *accept_connections(void *socket) {
    // keeps trying to accept any incoming connections
    while(1) {
        int client_sock;
        if((client_sock = accept((int)socket, NULL, NULL)) == -1) {
            DBG(printf("[ACCEPTOR THREAD %ld]: Connection requested to listinening socket %d REFUSED: %s\n", pthread_self(), (int)socket, strerror(errno)));
            return (void*)1;
        }

        // All went well: notify it if debug is on
        DBG(printf("[ACCEPTOR THREAD %ld]: Connection to client on socket %d ACCEPTED\n", pthread_self(), client_sock));

        // a thread is spawned to handle the connection on client_sock
        pthread_t worker;
        // the worker thread needs the socket used by the accepted client to read & write
        if(pthread_create(&worker, NULL, work, (void*)client_sock) == -1) {
            DBG(printf("[ACCEPTOR THREAD %ld]: Cannot spawn worker thread: %s", pthread_self(), strerror(errno)));
            return (void*)2;
        }
        // the thread just created is detatched, so that it can terminate indipendently of
        // this thread
        if(pthread_detach(worker) == -1) {
            DBG(printf("[ACCEPTOR THREAD %ld]: Cannot detach worker thread: %s", pthread_self(), strerror(errno)));
            return (void*)3;
        }

        // keep on trying to accept connections
    }
    return (void*)0;
}
