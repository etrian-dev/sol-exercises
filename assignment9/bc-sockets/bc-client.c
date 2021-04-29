/*
 * Realizzare in C un programma client ed un programma server. I due processi interagiscono
 * utilizzando socket AF_UNIX. Il client apre una connessione verso il server ed invia
 * richieste (sotto forma di stringhe) corrispondenti ad operazioni aritmetiche inserite
 * dall'utente (es. 2+1, 3/4 + 2/7, sqrt(5) + 3.14, …). Il server gestisce una connessione
 * alla volta verso un client. Il client, dopo l'invio della stringa contenente l'operazione
 * attende il risultato dal server prima di inviare una nuova richiesta. Per eseguire
 * l'operazione, il server lancia un processo che esegue la calcolatrice testuale 'bc'.
 * Il messaggio di risposta del server è una stringa contenente il risultato calcolato.
 * La sequenza di operazioni termina quando il client riceve in input la stringa “quit”
 * dall'utente che fa chiudere la connessione con il server. Il server si rimette in
 * attesa di ricevere una nuova connessione.
 */

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

// macro to wipe out debug prints from release executables
#if defined(DEBUG)
#define DBG(X) X
#else
#define DBG(X)
#endif

// maximum socket path lenght (socket path in argv[1])
#define PATHLEN_MAX 108
// max expression lenght to send trough the socket. Just for easiness of allocation
#define MSG_MAXLEN 100
// define maximum number of connection tries done by the client before giving up
#define MAXTRIES 100

int main(int argc, char **argv) {
    if(argc != 2) { printf("Usage: %s <sockpath>\n", argv[0]);}
    else {
        // sanity check on the sockets' filename to prevent overflow
        assert(strlen(argv[1]) < PATHLEN_MAX);

        // create a socket to connect to the server
        int mysock;
        if((mysock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
            perror("Cannot create socket");
            return 1;
        }

        struct sockaddr_un address;
        address.sun_family = AF_UNIX;
        strncpy(address.sun_path, argv[1], strlen(argv[1]) + 1);

        int tries = 0;
        // now that I know where to connect, call connect() (hopefully someone's listening)
        while(tries < MAXTRIES) {
            if(connect(mysock, (struct sockaddr *)&address, sizeof(address)) == -1) {
                // if the connection failed for some reason
                printf("Cannot connect to socket %d on address \"%s\": %s\n", mysock, argv[1], strerror(errno));
                tries++;
            }
            // otherwise the connection was estabilished, so it must exit the loop
            else {
                printf("[CLIENT %d]: Connected on socket %d\n", getpid(), mysock);
                break;
            }
        }
        if(tries == MAXTRIES) {
            exit(-1);
        }

        // connected to socket: read strings (expressions) and send them to the server
        // until "quit" is inputted
        char *expression = calloc(MSG_MAXLEN, sizeof(char));
        if(!expression) {perror("Alloc error"); exit(-1);}

        char *reply = malloc(MSG_MAXLEN * sizeof(char));
        if(!reply) {perror("Alloc error");exit(-1);}

        int mlen; // stores the lenght of the expression to be sent (return value of read())
        do {
            // read a new expression from stdin
            mlen = read(0, expression, MSG_MAXLEN);
            if(mlen == -1) {
                DBG(printf("[CLIENT %d]: Cannot read from stdin -> quitting: %s\n", getpid(), strerror(errno)));
                write(mysock, "quit", 5);
                strncpy(expression, "quit", 5);
            }
            // write the expression to stdout as well
            DBG(printf("[CLIENT %d]: sent \"%s\" from socket %d\n", getpid(), expression, mysock));

            int ret;
            // write the expression to the socket
            ret = write(mysock, expression, mlen);
            printf("Write expr: %d\n", ret);

            // wait for the reply from the server using a blocking read() on the socket
            ret = read(mysock, reply, MSG_MAXLEN);
            printf("Read res: %d\n", ret);

            // print the result of the expression to stdout
            printf("Risultato: %s\n", reply);

        } while(strncmp(expression, "quit", 4) != 0);
        // if "quit" was the expression, then the client exits from the loop and terminates

        // notify on stdout that the connection ended
        DBG(printf("[CLIENT %d]: communication on socket %d ended\n", getpid(), mysock));

        // then cleanup
        free(expression);
        free(reply);
        close(mysock);
    }
    return 0;
}
