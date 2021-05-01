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
#include <ctype.h>

// function used to initialize a socket for the server
int sock_init(char *address, size_t len);
// accept a connection
int accept_connection(const int serv_sock);

int capitalize(char *dest, size_t dest_len, char *string, size_t len) {
    size_t i;
    size_t bound = (dest_len < len ? dest_len : len);
    for(i = 0; i <= bound; i++) {
        dest[i] = toupper(string[i]);
    }
    return (int)i;
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
    char *result = calloc(BUFSZ, sizeof(char));
    if(!result) {
        perror("Alloc error");
        exit(-1);
    }
    
    // this thread receives the socket where the connections are coming as a parameter
    // for each accepted connection, a detached thread is spawned and it loops
    // keeps trying to accept any incoming connections
    int fd;
    int nclients = 0;
    while(1) {
        rdset = fd_read;
        // the last param is the timeout: if NULL blocks until an fd is available
        if(select(max_fd_idx, &rdset, NULL, NULL, NULL) == -1) {
            DBG(printf("[SERVER %d]: Failed to select a descriptor: %s\n", getpid(), strerror(errno)));
        }

        // scan all fds to see which were ready
        for(fd = 0; fd < max_fd_idx; fd++) {
            if(fd == sock_fd && FD_ISSET(fd, &rdset)) {
                // the server is ready to accept a new connection
                int newsock = accept_connection(sock_fd);
                FD_SET(newsock, &fd_read);
                // update the maximum fd
                if(max_fd_idx < newsock + 1) {
                    max_fd_idx = newsock + 1;
                }
                nclients++;
                printf("#clients connected: %d\n", nclients);
            }
            else if(FD_ISSET(fd, &rdset)) {
                // Connection estabilished: read strings from the client on the socket
                // and capitalize them
                int ret = read(fd, str, BUFSZ);
                if(ret == -1) {
                    perror("read() error. Exiting");
                    exit(-1);
                }
                
                if(strcmp(str, "quit\n") != 0) {
                    int reslen = capitalize(result, BUFSZ, str, ret);
                    // now send the capitalized string trough the socket to the client
                    write(fd, result, reslen);
                }
                else {
                    close(fd);
                    FD_CLR(fd, &fd_read); // remove from the tracked sockets  
                    nclients--;
                    printf("#clients connected: %d\n", nclients);
                }
                
                // clean internal buffers for the next calls
                memset(str, (char)0, BUFSZ);
                memset(result, (char)0, BUFSZ);
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
    }

    // returns the last created socket
    return newsock;
}
