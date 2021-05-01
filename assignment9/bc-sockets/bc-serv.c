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

// the socket shared by the client and the server (hardcoded)
#define SOCKET "sock"
// defines the buffer size
#define BUFSZ 1000

// The program is a server using sockets to communicate with client processes in solving
// expressions. It receives exactly one argument (a string) which is path to the
// socket the server is listening to
int main(int argc, char **argv) {
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
        // connect pipes to receive input from the parent and send output to it
        if(dup2(send_op[0], 0) == -1) {
            perror("Cannot dup pipe fd");
            exit(-1);
        }
        if(dup2(receive_res[1], 1) == -1) {
            perror("Cannot dup pipe fd");
            exit(-1);
        }
        // dup child's stderr to be redirected trough the pipe as well
        if(dup2(receive_res[1], 2) == -1) {
            perror("Cannot dup pipe fd");
            exit(-1);
        }
        // then close all pipe fds so that only stdin and stdout are open in this process
        if( close(send_op[0]) == -1
            || close(send_op[1]) == -1
            || close(receive_res[0]) == -1
            || close(receive_res[1]) == -1)
        {
            perror("Some fd cannot be closed");
            exit(-1);
        }

        // options -lq are needed to have the math library and no welcome message
        execl("/usr/bin/bc", "/usr/bin/bc", "-lq", NULL);
        // exec returns only if it fails
        perror("Cannot exec bc(1)");
        exit(1);
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
    strncpy(address.sun_path, argv[1], strlen(argv[1]) + 1);

    // the socket then must be binded to an address. in this case the address is
    // specified by the argument argv[1]
    if(bind(sock_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        printf("Cannot bind socket %d to address \"%s\": %s\n", sock_fd, argv[1], strerror(errno));
        return 2;
    }


    // make the server listen for incoming connections (one at a time), then connect
    // accept the connection and do i/o with that socket
    while(1) {
        // the maximum number of queued connections is 0, so one client at a time
        // can be connected to the server
        if(listen(sock_fd, 0) == -1) {
            printf("Cannot listen on socket %d: %s\n", sock_fd, strerror(errno));
            return 2;
        }

        // the socket sock_fd now is listening for connections, and the accept() call
        // IN THIS CASE blocks the process until a connection is received, and accepts it
        struct sockaddr_un conn_addr; // this struct stores the accepted connection infos on success
        int len;
        int conn_sock; // the new socket where the communication happens.
        // This socket is connected to another in the client process
        if((conn_sock = accept(sock_fd, (struct sockaddr *)&conn_addr, (socklen_t *)&len)) == -1) {
            printf("Cannot accept connection on socket %d: %s\n", sock_fd, strerror(errno));
            return 3;
        }

        // Connection estabilished: receive operations from the client and send
        // them to the child and receive results to send to the client

        // the read buffer conatins operations requested by the client.
        // If the string is "quit", it closes the connection with the client
        char *buf = calloc(BUFSZ, sizeof(char));
        if(!buf) {perror("Alloc error"); exit(-1);}

        int msglen = read(conn_sock, buf, BUFSZ);
        if(msglen == -1) {perror("read() error. Exiting"); exit(-1);}

        while(strncmp(buf, "quit", 4) != 0) {
            printf("[SERVER %d]: received message \"%s\" from client on socket %d\n", getpid(), buf, conn_sock);

            // the message received is sent trough the pipe to the child process bc
            // the fd to be used to do this is send_op[1]
            // write lenght is limited by the # of bytes read by read()
            if(write(send_op[1], buf, msglen) == -1) {
                DBG(printf("[SERVER %d]: write to pipe %d failed: %s\n", getpid(), send_op[1], strerror(errno)));
                exit(-1);
            }

            // wait for the reply from bc, using a blocking call to read() on the pipe
            // receive_res[1]
            int res_len;
            if((res_len = read(receive_res[0], buf, BUFSZ)) == -1) {
                DBG(printf("[SERVER %d]: read from pipe %d failed: %s\n", getpid(), receive_res[0], strerror(errno)));
                exit(-1);
            }
            buf[res_len - 1] = '\0'; // if for some reason the terminator is not set, set it

            // then send the result on the socket to the client
            if(write(conn_sock, buf, res_len) == -1) {
                DBG(printf("[SERVER %d]: write to socket %d failed: %s\n", getpid(), conn_sock, strerror(errno)));
                exit(-1);
            }

            // clean the buffer
            memset(buf, (char)0, BUFSZ);

            // then read the next operation
            msglen = read(conn_sock, buf, BUFSZ);
            if(msglen == -1) {perror("read() error. Exiting"); exit(-1);}
            printf("Read %d bytes\n", msglen);
        }

        // the client requested the connection termination: close the socket
        if(close(conn_sock) == -1) {
            DBG(printf("[SERVER %d]: Cannot terminate connection on socket %d: %s\n", getpid(), conn_sock, strerror(errno)));
            exit(-1);
        }

        // free the client buffer: we don't want buffers to be shared by different
        // clients
        free(buf);
    }

    // then close everything  & free
    close(sock_fd);
    close(send_op[1]);
    close(receive_res[0]);
    // then unlink the socket file to remove it (explained at man 2 unlink)
    if(unlink(argv[1]) == -1) {
        DBG(printf("[SERVER %d]: cannot remove socket \"%s\": %s\n", getpid(), argv[1], strerror(errno)));
        return 4;
    }
    return 0;
}
