/*
 * Realizzare in C un programma client ed un programma server. I due processi interagiscono
 * utilizzando socket AF_UNIX. Il client apre una connessione verso il server ed invia
 * richieste (sotto forma di stringhe) corrispondenti ad operazioni aritmetiche inserite
 * dall'utente (es. 2+1, 3/4 + 2/7, sqrt(5) + 3.14, …). Il server gestisce una connessione
 * alla volta verso un client. Il client, dopo l'invio della stringa contenente l'operazione
 * attende il risultato dal server prima di inviare una nuova richiesta. Per eseguire
 * l'operazione, il server lancia un processo che esegue la calcolatrice testuale 'bc'.
 * Il messaggio di risposta del server è una stringa contenente il risultato calcolato.
 * La sequenza di operazioni termina quando il client riceve in buffer la stringa “quit”
 * dall'utente che fa chiudere la connessione con il server. Il server si rimette in
 * attesa di ricevere una nuova connessione.
 */

// my header
#include <util.h>
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

// This server creates a socket defined by the macro SOCKET (see util.h)
// and listens for connections from clients. Only one connection at a time is permitted
// and the service provided is bc(1), trough a child process that communicates via
// anonymous pipes with the server
int main(int argc, char **argv) { // no args are used actually
    // unlink the socket, if already present in the filesystem
    unlink(SOCKET);
    
   // create pipes to communicate with the child process executing bc
    int send_op[2]; // pipe used to send expressions to bc
    int receive_res[2]; // pipe to receive results
    if(pipe(send_op) == -1) {
        perror("Pipes cannot be created");
        exit(-1);
    }
    if(pipe(receive_res) == -1) {
        perror("Pipes cannot be created");
        exit(-1);
    }

    // fork a process to exec bc
    pid_t bc;
    if((bc = fork()) == -1) {
        perror("Parent cannot fork()");
        exit(-1);
    }
    // code executed in the child process: does not return on success
    if(bc == 0) {
        child(send_op, receive_res);
    }
    
    // Code executed by the parent process
    
    // close unused pipe fds
    if(close(send_op[0]) == -1 || close(receive_res[1]) == -1) {
        perror("Some pipe fd cannot be closed");
        exit(-1);
    }

    /*
    * This section creates a socket of type AF_UNIX at the path specified by SOCKET
    * and listens for incoming connections (only one client can be connected at any
    * time: other clients trying to connect will wait until their request is accepted)
    */

    int sock_fd; // this is the socket used to listen for connections
    if((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Cannot create socket");
        return 1;
    }
    struct sockaddr_un address; // this struct is filled with the socket address
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET, strlen(SOCKET) + 1); // safe: it's a compile-time constant
    
    // the socket is binded to SOCKET
    if(bind(sock_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        DBG(printf("Cannot bind socket %d to address \"%s\": %s\n", sock_fd, argv[1], strerror(errno)));
        return 2;
    }

    // the maximum number of queued clients waiting to be accepted is SOMAXCONN
    if(listen(sock_fd, SOMAXCONN) == -1) {
        DBG(printf("Cannot listen on socket %d: %s\n", sock_fd, strerror(errno)));
        return 2;
    }

    // make the server listen for incoming connections (one at a time) indefinitely (killed by CTRL+C)
    // Once a connection is accepted, it will have its own new socket to perform
    // I/O with the client
    while(1) {
        int conn_sock; // the new socket where the communication happens
        if((conn_sock = accept(sock_fd, NULL, NULL)) == -1) {
            DBG(printf("Cannot accept connection on socket %d: %s\n", sock_fd, strerror(errno)));
            return 3;
        }

        // Connection estabilished: receive operations from the client trough the socket
        // send them to the child and receive results to be sent to the client

        // the buffer contains operations requested by the client and results read
        // from the calculator
        char *buffer = calloc(BUFSZ, sizeof(char));
        if(!buffer) {perror("Alloc error"); exit(-1);}

        // read a message first
        int msglen = read(conn_sock, buffer, BUFSZ);
        if(msglen == -1) {perror("read() error. Exiting"); exit(-1);}
        
        // if the message sent from the client is "quit\n", then the server closes
        // the connection with this client and loops to accept()
        // only test characters up to msglen, otherwise the buffer's contents from previous reads might cause unintended behaviour
        while(strncmp(buffer, "quit\n", msglen) != 0) {
            DBG(printf("[SERVER %d]: received message \"%s\" from client on socket %d\n", getpid(), buffer, conn_sock));

            // the message received is sent trough the pipe to the child process bc
            // the fd to be used to do this is send_op[1]
            // write lenght is limited by the # of bytes read by read()
            if(write(send_op[1], buffer, msglen) == -1) {
                DBG(printf("[SERVER %d]: write to pipe %d failed: %s\n", getpid(), send_op[1], strerror(errno)));
                exit(-1);
            }

            // wait for the reply from bc, using a blocking call to read() on the pipe
            // receive_res[0]
            if((msglen = read(receive_res[0], buffer, BUFSZ)) == -1) {
                DBG(printf("[SERVER %d]: read from pipe %d failed: %s\n", getpid(), receive_res[0], strerror(errno)));
                exit(-1);
            }

            // then send the result on the socket to the client
            if(write(conn_sock, buffer, msglen) == -1) {
                DBG(printf("[SERVER %d]: write to socket %d failed: %s\n", getpid(), conn_sock, strerror(errno)));
                exit(-1);
            }

            // then read the next expression
            msglen = read(conn_sock, buffer, BUFSZ);
            if(msglen == -1) {perror("read() on socket error. Exiting"); exit(-1);}
        }

        // the client requested the connection termination: close the socket
        if(close(conn_sock) == -1) {
            DBG(printf("[SERVER %d]: Cannot terminate connection on socket %d: %s\n", getpid(), conn_sock, strerror(errno)));
            exit(-1);
        }

        // free the client buffer: we don't want buffers to be shared by different clients
        free(buffer);
    }

    // then close every fd still opened
    if( close(sock_fd) == -1
       || close(send_op[1]) == -1
       || close(receive_res[0]) == -1)
    {
        perror("Failed to close some fd");
    }
    
    return 0;
}
