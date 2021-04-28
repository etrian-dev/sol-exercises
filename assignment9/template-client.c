// unix sokets example program

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

#define MSG_MAXLEN 100

#define PATHLEN_MAX 108

int main(int argc, char **argv) {
    assert(argc == 2 && strlen(argv[1]) < PATHLEN_MAX);

    // first create a socket in the server and get a fd for it
    int mysock;
    // creates a socket to communicate between local processes with a stream type
    // "reliable, two-way, connection-based byte stream" (see man 2 socket)
    if((mysock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Cannot create socket");
        return 1;
    }

    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, argv[1], strlen(argv[1]) + 1);

    // now that I know where to connect, call connect() (hopefully someone's listening)
    if(connect(mysock, (struct sockaddr *)&address, sizeof(address)) == -1) {
        printf("Cannot connect to socket %d on address \"%s\": %s\n", mysock, argv[1], strerror(errno));
        return 3;
    }

    // connected to socket: send messages to the server

    char *message = calloc(MSG_MAXLEN, sizeof(char));
    if(!message) {perror("Alloc error"); exit(-1);}
    int mlen; // stores the lenght of the message to be sent (return value of read())
    do {
        // read a new message from stdin
        mlen = read(0, message, MSG_MAXLEN);
        if(mlen == -1) {
            printf("[%d]: read() error: quitting: %s\n", getpid(), strerror(errno));
            write(mysock, "quit", 5);
            strcpy(message, "quit");
        }
        // strip that annoying newline
        message[mlen - 1] = '\0';
        // send the message to stdout as well
        printf("[CLIENT %d]: sent \"%s\" from socket %d\n", getpid(), message, mysock);

        // write the message to the socket
        if(write(mysock, message, mlen) == -1) {
            printf("[%d]: write() error: exiting: %s\n", getpid(), strerror(errno));
            exit(-1);
        }
    } while(strncmp(message, "quit", 4) != 0);
    // "quit" has been sent to the socket, so exit the loop

    // notify on stdout that the connection ended
    printf("[CLIENT %d]: communication on socket %d ended\n", getpid(), mysock);

    // then cleanup
    free(message);
    close(mysock);

    return 0;
}
