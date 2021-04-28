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

#define PATHLEN_MAX 108

#define READ_BUFSZ 1000

int main(int argc, char **argv) {
    assert(argc == 2 && strlen(argv[1]) < PATHLEN_MAX);

    // first create a socket in the server and get a fd for it
    int sock_fd;
    // creates a socket to communicate between local processes with a stream type
    // "reliable, two-way, connection-based byte stream" (see man 2 socket)
    if((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Cannot create socket");
        return 1;
    }

    // the socket then must be binded to an address. in this case the address is
    // hardcoded as argv[1]
    // creates a sockaddr struct used by the AF_UNIX socket (see man 7 unix)
    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, argv[1], strlen(argv[1]) + 1);

    if(bind(sock_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        printf("Cannot bind socket %d to address \"%s\": %s\n", sock_fd, argv[1], strerror(errno));
        return 2;
    }

    // now that the socket is binded to some file, make it listen for incoming connections
    // the maximum number of queued connections is 1 (for testing purposes)
    if(listen(sock_fd, 1) == -1) {
        printf("Cannot listen on socket %d: %s\n", sock_fd, strerror(errno));
        return 2;
    }

    // the socket sock_fd now is listening for connections, and the accept() call
    // IN THIS CASE blocks the process until a connection is received, and accepts it
    struct sockaddr_un conn_addr;
    size_t len;
    int conn_sock = accept(sock_fd, (struct sockaddr *)&conn_addr, (socklen_t *)&len);
    // if it succeds sets conn_sock to a new socket to be used for communication
    // and fills the structure conn_addr with the client's address and len with its addr's lenght
    if(conn_sock == -1) {
        printf("Cannot accept connection on socket %d: %s\n", sock_fd, strerror(errno));
        return 3;
    }

    // Connection estabilished: receive messages and print them until the "quit" message is
    // received
    char *buf = calloc(READ_BUFSZ, sizeof(char));
    if(!buf) {perror("Alloc error"); exit(-1);}

    int msglen = read(conn_sock, buf, READ_BUFSZ);
    if(msglen == -1) {perror("read() error. Exiting"); exit(-1);}
    printf("Read %d bytes\n", msglen);

    while(strncmp(buf, "quit", 4) != 0) {
        printf("[SERVER %d]: received message \"%s\" from client on socket %d\n", getpid(), buf, conn_sock);

        // clean the buffer
        memset(buf, (char)0, msglen);

        msglen = read(conn_sock, buf, READ_BUFSZ);
        if(msglen == -1) {perror("read() error. Exiting"); exit(-1);}
        printf("Read %d bytes\n", msglen);
    }

    // then close everything
    free(buf);
    close(conn_sock);
    close(sock_fd);

    // then unlink the socket file to remove it (explained at man 2 unlink)
    if(unlink(argv[1]) == -1) {
        printf("[SERVER %d]: cannot remove socket \"%s\": %s\n", getpid(), argv[1], strerror(errno));
        return 4;
    }

    return 0;
}
