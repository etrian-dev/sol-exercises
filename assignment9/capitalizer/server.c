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

// my header
#include <util.h>
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

int sock_init(char *address, size_t len);
void *accept_connections(void *socket);

int main(int argc, char **argv) {
    // create the socket and bind it to a known address
    int sock_fd;
    if((sock_fd = sock_init(ADDR, strlen(ADDR))) == -1) {
        DBG(printf("[SERVER %d]: Cannot create server socket: %s\n", getpid(), strerror(errno)));
        return 1;
    }

    // A thread is spawned to handle accepting connections from clients
    pthread_t acceptor;
    if(pthread_create(&acceptor, NULL, accept_connections, (void*)sock_fd) == -1) {
        DBG(printf("[SERVER %d]: Cannot create acceptor thread: %s\n", getpid(), strerror(errno)));
        return 1;
    }

    pthread_join(acceptor, NULL);

    return 0;
}
