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

// The program is a server using sockets to communicate with client processes in solving
// expressions. It receives exactly one argument (a string) which is path to the
// socket the server is listening to
int main(int argc, char **argv) {
    // unlink the socket, if already present
    unlink(SOCKET);
    
    /*
    * first create a child process executing bc, and declare pipes to redirect its i/o
    * so that operations sent to the server can be sent to the bc process and
    * the result received from it can be sent through a socket to the connected client
    */
    int send_op[2]; // pipe used to send expressions to bc
    int receive_res[2]; // pipe to receive results
    if(pipe(send_op) == -1) {
        perror("Pipes cannot be created");
    }
    if(pipe(receive_res) == -1) {
        perror("Pipes cannot be created");
    }

    // fork a process to exec bc
    pid_t bc;
    if((bc = fork()) == -1) {
        perror("fork() error: request ignored");
        exit(-1);
    }
    // code executed in the child process
    if(bc == 0) {
        child(send_op, receive_res);
    }
    // Code executed by the parent process
    // close unused pipe fds
    else{
        if(close(send_op[0]) == -1 || close(receive_res[1]) == -1) {
            perror("Some fd cannot be closed");
            exit(-1);
        }
    }

    /*
    * This section creates a socket of type AF_UNIX at the path specified by argv[1]
    * and listens for incoming connections. Once a connection is estabilished, then
    * the client sends operations through the socket and expects results to arrive trough it
    */

    // first create a socket's file descriptor
    int sock_fd;
    // creates a local socket, with two-way connection-based communication
    // see man 2 socket for further info
    if((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Cannot create socket");
        return 1;
    }

    struct sockaddr_un address; // this struct is filled with the path and the socket type
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET, strlen(SOCKET) + 1);

    // the socket then must be binded to an address. in this case the address is
    // specified by the argument argv[1]
    if(bind(sock_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        DBG(printf("Cannot bind socket %d to address \"%s\": %s\n", sock_fd, argv[1], strerror(errno)));
        return 2;
    }

    // the maximum number of queued connections is 0, so one client at a time
    // can be connected to the server
    if(listen(sock_fd, 0) == -1) {
        DBG(printf("Cannot listen on socket %d: %s\n", sock_fd, strerror(errno)));
        return 2;
    }

    // make the server listen for incoming connections (one at a time), then connect
    // accept the connection and do i/o with that socket
    while(1) {
        // the socket sock_fd now is listening for connections, and the accept() call
        int conn_sock; // the new socket where the communication happens.
        // This socket is connected to another in the client process
        if((conn_sock = accept(sock_fd, NULL, NULL)) == -1) {
            DBG(printf("Cannot accept connection on socket %d: %s\n", sock_fd, strerror(errno)));
            return 3;
        }

        // Connection estabilished: receive operations from the client and send
        // them to the child and receive results to send to the client

        // the read buffer conatins operations requested by the client.
        // If the string is "quit", it closes the connection with the client
        char *input = calloc(BUFSZ, sizeof(char));
        if(!input) {perror("Alloc error"); exit(-1);}
        
        char *output = calloc(BUFSZ, sizeof(char));
        if(!output) {perror("Alloc error"); exit(-1);}

        int msglen = read(conn_sock, input, BUFSZ);
        if(msglen == -1) {perror("read() error. Exiting"); exit(-1);}

        while(strncmp(input, "quit\n", msglen) != 0) {
            DBG(printf("[SERVER %d]: received message \"%s\" from client on socket %d\n", getpid(), input, conn_sock));

            // the message received is sent trough the pipe to the child process bc
            // the fd to be used to do this is send_op[1]
            // write lenght is limited by the # of bytes read by read()
            if(write(send_op[1], input, msglen) == -1) {
                DBG(printf("[SERVER %d]: write to pipe %d failed: %s\n", getpid(), send_op[1], strerror(errno)));
                exit(-1);
            }

            // wait for the reply from bc, using a blocking call to read() on the pipe
            // receive_res[0]
            int res_len;
            if((res_len = read(receive_res[0], output, BUFSZ)) == -1) {
                DBG(printf("[SERVER %d]: read from pipe %d failed: %s\n", getpid(), receive_res[0], strerror(errno)));
                exit(-1);
            }

            // then send the result on the socket to the client
            if(write(conn_sock, output, res_len) == -1) {
                DBG(printf("[SERVER %d]: write to socket %d failed: %s\n", getpid(), conn_sock, strerror(errno)));
                exit(-1);
            }
            
            // clean the buffers before the next read

            // then read the next operation
            msglen = read(conn_sock, input, BUFSZ);
            if(msglen == -1) {perror("read() on socket error. Exiting"); exit(-1);}
        }

        // the client requested the connection termination: close the socket
        if(close(conn_sock) == -1) {
            DBG(printf("[SERVER %d]: Cannot terminate connection on socket %d: %s\n", getpid(), conn_sock, strerror(errno)));
            exit(-1);
        }

        // free the client buffer: we don't want buffers to be shared by different
        // clients
        free(input);
        free(output);
    }

    // then close everything  & free
    close(sock_fd);
    close(send_op[1]);
    close(receive_res[0]);
    
    return 0;
}
