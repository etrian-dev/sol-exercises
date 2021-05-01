// worker thread: processes requests from the client connected on socket client_sock
// and terminates when "quit" is sent. The thread sends to bc(1) (forked elsewhere) the

// my headers
#include <util.h>
// threading header
#include <pthread.h>
// sockets headers
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
// I/O calls
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define WHICH_BC "/usr/bin/bc"

int capitalize(char *dest, size_t dest_len, char *string, size_t len) {
    size_t i;
    size_t bound = (dest_len < len ? dest_len : len);
    for(i = 0; i <= bound; i++) {
        dest[i] = toupper(string[i]);
    }
    return (int)i;
}

void *work(void *client_sock) {
    // Connection estabilished: read strings from the client on the socket
    // and capitalize them
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

    int msglen = read((int)client_sock, str, BUFSZ);
    if(msglen == -1) {
        perror("read() error. Exiting");
        exit(-1);
    }

    while(strncmp(str, "quit", 4) != 0) {
        DBG(printf("[WORKER %ld]: Read str \"%s\" from client on socket %d\n", pthread_self(), str, (int)client_sock));

        int reslen = capitalize(result, BUFSZ, str, msglen);

        // now send the capitalized string trough the socket to the client
        if(write((int)client_sock, result, reslen) == -1) {
            DBG(printf("[WORKER %ld]: Sending result to the client failed: %s\n", pthread_self(), strerror(errno)));
        }

        // clean buffers?
        memset(str, (char)0, BUFSZ);
        memset(result, (char)0, BUFSZ);

        // then read another expression and loop
        msglen = read((int)client_sock, str, BUFSZ);
        if(msglen == -1) {
            DBG(printf("[WORKER %ld]: read() error. Exiting\n", pthread_self()));
            exit(-1);
        }
    }

    // the client sent the string "quit", so bc terminated. The thread frees resources
    free(str);
    free(result);
    // close the socket used for the connection
    close((int)client_sock);

    // then exits
    return (void*)0;
}
