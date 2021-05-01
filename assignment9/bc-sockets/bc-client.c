/*
 * Realizzare in C un programma client ed un programma serv_sock. I due processi interagiscono
 * utilizzando socket AF_UNIX. Il client apre una connessione verso il serv_sock ed invia
 * richieste (sotto forma di stringhe) corrispondenti ad operazioni aritmetiche inserite
 * dall'utente (es. 2+1, 3/4 + 2/7, sqrt(5) + 3.14, …). Il serv_sock gestisce una connessione
 * alla volta verso un client. Il client, dopo l'invio della stringa contenente l'operazione
 * attende il risultato dal serv_sock prima di inviare una nuova richiesta. Per eseguire
 * l'operazione, il serv_sock lancia un processo che esegue la calcolatrice testuale 'bc'.
 * Il messaggio di risposta del serv_sock è una stringa contenente il risultato calcolato.
 * La sequenza di operazioni termina quando il client riceve in input la stringa “quit”
 * dall'utente che fa chiudere la connessione con il serv_sock. Il serv_sock si rimette in
 * attesa di ricevere una nuova connessione.
 */

// headers for sockets and syscalls
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

// the socket shared by the client and the serv_sock (hardcoded)
#define SOCKET "sock"
// max lenght of an expression that can be sent trough the socket
#define MSG_MAXLEN 100
// define maximum number of connection tries done by the client before giving up
#define MAXTRIES 100

int main(int argc, char **argv) {
    // create a socket to connect to the serv_sock
    int serv_sock;
    if((serv_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        DBG(printf("[CLIENT %d]: Cannot create socket: %s\n", getpid(), strerror(errno)));
        return 1;
    }

    // now that I know where to connect, call connect() (hopefully someone's listening)
    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET, strlen(SOCKET) + 1);
    int tries = 0;
    while(tries < MAXTRIES) {
        if(connect(serv_sock, (struct sockaddr *)&address, sizeof(address)) == -1) {
            // the connection failed for some reason: retry
            tries++;
        }
        // otherwise the connection was estabilished, so it must exit the loop
        else {
            DBG(printf("[CLIENT %d]: Connected on socket %d\n", getpid(), serv_sock));
            break;
        }
    }
    if(tries == MAXTRIES) {
        DBG(printf("[CLIENT %d]: Cannot connect to socket \"%s\" on address: %s\n", getpid(), SOCKET, strerror(errno)));
        return 1;
    }

    // connected to socket: read expressions from stdin and send them to the serv_sock
    // until "quit" is specified
    char *expression = calloc(MSG_MAXLEN, sizeof(char));
    if(!expression) {perror("Alloc error"); exit(-1);}

    char *reply = malloc(MSG_MAXLEN * sizeof(char));
    if(!reply) {perror("Alloc error");exit(-1);}

    int mlen; // stores the lenght of the expression to be sent (return value of read())
    do {
        // read a new expression from stdin
        mlen = read(0, expression, MSG_MAXLEN);
        if(mlen == -1) {
            DBG(printf("[CLIENT %d]: Cannot read expression from stdin -> quitting: %s\n", getpid(), strerror(errno)));
            write(serv_sock, "quit", 5);
            break;
        }

        int ret;
        // write the expression to the socket
        ret = write(serv_sock, expression, mlen);
        printf("Write expr: %d\n", ret);

        // wait for the reply from the serv_sock using a blocking read() on the socket
        ret = read(serv_sock, reply, MSG_MAXLEN);
        printf("Read res: %d\n", ret);

        // print the result of the expression to stdout
        printf("Risultato: %s\n", reply);

    } while(strncmp(expression, "quit", 4) != 0);
    // if "quit" was the expression, then the client exits from the loop and terminates

    // notify on stdout that the connection ended
    DBG(printf("[CLIENT %d]: communication on socket %d ended\n", getpid(), serv_sock));

    // then cleanup
    free(expression);
    free(reply);
    close(serv_sock);
    
    return 0;
}
