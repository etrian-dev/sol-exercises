/*
 * Realizzare un programma C che implementa un server che rimane sempre attivo in
 * attesa di richieste da parte di uno o piu' processi client su una socket di tipo
 * AF_UNIX. Ogni client richiede al server la trasformazione di tutti i caratteri di
 * una stringa da minuscoli a maiuscoli (es. ciao –> CIAO). Per ogni nuova connessione
 * il server lancia un thread POSIX che gestisce tutte le richieste del client
 * (modello “un thread per connessione”) e quindi termina la sua esecuzione quando
 * il client chiude la connessione.
 *
 * Per testare il programma implementare uno script bash che lancia N>10 clients
 * ognuno dei quali invia una o piu' richieste al server multithreaded.
 */

// my headers
#include <util.h>
// unix/linux syscalls
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
// std headers
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

// define maximum number of connection tries done by the client before giving up
#define MAXTRIES 100

int main(int argc, char **argv) {
    // create a socket to connect to the server
    int mysock;
    if((mysock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Cannot create socket");
        return 1;
    }

    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, ADDR, strlen(ADDR) + 1);

    int tries = 0;
    // now that I know where to connect, call connect() (hopefully someone's listening)
    while(tries < MAXTRIES) {
        if(connect(mysock, (struct sockaddr *)&address, sizeof(address)) == -1) {
            // if the connection failed for some reason
            printf("Cannot connect to socket %d on address \"%s\": %s\n", mysock, ADDR, strerror(errno));
            tries++;
        }
        // otherwise the connection was estabilished, so it must exit the loop
        else {
            break;
        }
    }

    // connected to socket: read strings (expressions) and send them to the server
    // until "quit" is inputted
    char *expression = calloc(BUFSZ, sizeof(char));
    if(!expression) {
        perror("Alloc error");
        exit(-1);
    }

    char *reply = malloc(BUFSZ * sizeof(char));
    if(!reply) {
        perror("Alloc error");
        exit(-1);
    }

    int mlen; // stores the lenght of the expression to be sent (return value of read())
    do {
        // read a new expression from stdin
        mlen = read(0, expression, BUFSZ);
        if(mlen == -1) {
            DBG(printf("[CLIENT %d]: Cannot read from stdin -> quitting: %s\n", getpid(), strerror(errno)));
            write(mysock, "quit", 5);
            strncpy(expression, "quit", 5);
        }
        // write the expression to stdout as well
        DBG(printf("[CLIENT %d]: sent \"%s\" from socket %d\n", getpid(), expression, mysock));

        // write the expression to the socket
        if(write(mysock, expression, mlen) == -1) {
            DBG(printf("[CLIENT %d]: Cannot write to the socket -> exiting: %s\n", getpid(), strerror(errno)));
            exit(-1);
        }

        // wait for the reply from the server using a blocking read() on the socket
        int res = read(mysock, reply, BUFSZ);
        if(res == -1) {
            perror("Cannot obtain result from socket");
            exit(-1);
        }
        else if(res == 0) {
            DBG(printf("[CLIENT %d]: Socket closed -> terminate\n", getpid()));
            break;
        }
        else {
            // print the result of the expression to stdout
            printf("Risultato: %s\n", reply);
        }
    } while(strcmp(expression, "quit\n") != 0);
    // if "quit" was the expression, then the client exits from the loop and terminates

    // notify on stdout that the connection ended
    DBG(printf("[CLIENT %d]: communication on socket %d ended\n", getpid(), mysock));

    // then cleanup
    free(expression);
    free(reply);
    close(mysock);

    return 0;
}
