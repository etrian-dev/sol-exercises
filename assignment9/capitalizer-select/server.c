/*
 * Realizzare un programma C che implementa un server che rimane sempre attivo in
 * attesa di richieste da parte di uno o piu' processi client su una socket di tipo
 * AF_UNIX. Ogni client richiede al server la trasformazione di tutti i caratteri di
 * una stringa da minuscoli a maiuscoli (es. ciao –> CIAO). Il server è sequenziale
 * e gestisce più client contemporaneamente utilizzando la syscall select
 *
 * Per testare il programma, lanciare piu' processi client ognuno dei quali invia una
 * o piu' richieste al server multithreaded.
 */

// my header
#include <util.h>
// syscall headers
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/select.h> // for select()
#include <unistd.h>
// std headers
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h> // to use toupper

// function used to initialize a socket for the server
int sock_init(char *address, size_t len);
// accept a connection
int accept_connection(const int serv_sock);

// little function used to capitalize string
void capitalize(char *string, size_t len) {
    size_t i;
    for(i = 0; i < len; i++) {
        string[i] = toupper(string[i]);
    }
}

int main(int argc, char **argv) {
    // first try to unlink the socket
    unlink(ADDR);

    // create the socket and bind it to a known address
    int sock_fd;
    if((sock_fd = sock_init(ADDR, strlen(ADDR))) == -1) {
        DBG(printf("[SERVER %d]: Cannot create server socket: %s\n", getpid(), strerror(errno)));
        return 1;
    }

    // initialize the set of fds controlled by select with the socket
    int max_fd_idx = sock_fd + 1;
    // the fd set from which the select reads is initialized to no fd
    fd_set fd_read, rdset;
    FD_ZERO(&fd_read);
    FD_SET(sock_fd, &fd_read);
    
    char *str = calloc(BUFSZ, sizeof(char));
    if(!str) {
        perror("Alloc error");
        exit(-1);
    }
    
    int fd;
    while(1) {
        // sets are modified by select, so they must be resetted each loop
        rdset = fd_read;
        // the server selects which file descriptors are ready for reading
        // the last param is the timeout: if NULL blocks until an fd is ready
        if(select(max_fd_idx, &rdset, NULL, NULL, NULL) == -1) {
            DBG(printf("[SERVER %d]: Failed to select a descriptor: %s\n", getpid(), strerror(errno)));
        }

        // scan all fds to see which were ready
        for(fd = 0; fd < max_fd_idx; fd++) {
            if(fd == sock_fd && FD_ISSET(fd, &rdset)) {
                // the server is ready to accept a new connection
                int client_sock;
                if((client_sock = accept_connection(sock_fd)) == -1) {
                    // failed to create a new connection
                    DBG(printf("[SERVER %d]: Failed to connect a client: %s\n", getpid(), strerror(errno)));
                    continue;
                }
                // include the socket in the readset
                FD_SET(client_sock, &fd_read);
                
                // update the maximum fd if needed
                if(max_fd_idx < client_sock + 1) {
                    max_fd_idx = client_sock + 1;
                }
            }
            else if(FD_ISSET(fd, &rdset)) {
                // the client socket is ready for reading, so a string is read
                int mlen = read(fd, str, BUFSZ);
                if(mlen == -1) {
                    perror("read() error. Exiting");
                    exit(-1);
                }
                
                if(strncmp(str, "quit\n", mlen) != 0) {
                    capitalize(str, mlen - 1);
                    // now send the capitalized string trough the socket to the client
                    write(fd, str, mlen);
                }
                else {
                    // close the socket
                    if(close(fd) == -1) {
                        perror("Cannot close the client socket");
                    }
                    FD_CLR(fd, &fd_read); // remove it from the set of tracked sockets  
                }
            }
        }
    }

    return 0;
}

// accept a connection
int accept_connection(const int serv_sock) {
    int newsock;
    if((newsock = accept(serv_sock, NULL, NULL)) == -1) {
        DBG(printf("[SERVER %d]: Failed to accept connection: %s\n", getpid(), strerror(errno)));
        return -1;
    }

    // returns the last created socket
    return newsock;
}
