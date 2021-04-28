/*
 * Realizzare un programma C che implementa un server che rimane sempre attivo in
 * attesa di richieste da parte di uno o piu' processi client su una socket di tipo
 * AF_UNIX. Ogni client richiede al server la trasformazione di tutti i caratteri di
 * una stringa da minuscoli a maiuscoli (es. ciao –> CIAO). Per ogni nuova connessione
 * il server lancia un thread POSIX che gestisce tutte le richieste del client
 * (modello “un thread per connessione”) e quindi termina la sua esecuzione quando
 * il client chiude la connessione.
 *
 * Per testare il programma, lanciare piu' processi client ognuno dei quali invia una
 * o piu' richieste al server multithreaded.
 */

// threads header
#include <pthread.h>
// syscall headers
#include <sys/types.h>
#include <unistd.h>
// std headers
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

// macro to wipe out debug prints from release executables
#if defined(DEBUG)
#define DBG(X) X
#else
#define DBG(X)
#endif


// known path to create the socket file
#define ADDR "./sock"

// maximum socket path lenght
#define PATHLEN_MAX 108

#define READ_BUFSZ 1000

int sock_init(char *address, size_t len);
void *accept_connections(void *socket);

int main(int argc, char **argv) {
    // create the socket and bind it to a known address
    int sock_fd = sock_init(ADDR, strlen(ADDR));

    // A dedicated thread is spawned to handle accepting connections
    pthread_t acceptor;
    if(pthread_create(&acceptor, NULL, accept_connections, (void*)sock_fd) == -1) {
        DBG(printf("[SERVER %d]: Cannot create acceptor thread: %s\n", getpid(), strerror(errno)));
        return 1;
    }

    // then unlink the socket file to remove it (explained at man 2 unlink)
    //if(unlink(argv[1]) == -1) {
        //printf("[SERVER %d]: cannot remove socket \"%s\": %s\n", getpid(), argv[1], strerror(errno));
        //return 4;
    //}

    pthread_join(acceptor, NULL);

    return 0;
}
