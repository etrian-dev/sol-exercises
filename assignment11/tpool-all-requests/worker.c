// worker thread: processes requests from the client connected on socket client_sock
// and terminates when "quit\n" is sent.

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

// little function used to capitalize string
void capitalize(char *string, size_t len) {
    size_t i;
    for(i = 0; i < len; i++) {
        string[i] = toupper(string[i]);
    }
}

void *work(void *clients_q) {
    struct Sync_Queue *Q = (struct Sync_Queue *)clients_q;
    while(1) {
        // while there are no items in the queue: wait
        struct Queue *elem = NULL;
        while((elem = pop(&(Q->head), &(Q->tail))) == NULL) {
            pthread_cond_wait(&new_client, &mux);
        }

        // now extract the new client's socket to listen to
        int client_sock = (int)elem->data_ptr;
        // then free the message from the queue
        free(elem);

        // Connection estabilished: read strings from the client on the socket
        // and capitalize them
        char *str = calloc(BUFSZ, sizeof(char));
        if(!str) {
            perror("Alloc error");
            exit(-1);
        }

        int msglen = read(client_sock, str, BUFSZ);
        if(msglen == -1) {
            perror("read() error. Exiting");
            exit(-1);
        }

        while(strncmp(str, "quit\n", msglen) != 0) {
            DBG(printf("[WORKER %ld]: Read str \"%s\" from client on socket %d\n", pthread_self(), str, client_sock));

            capitalize(str, msglen - 1);

            // now send the capitalized string trough the socket to the client
            if(write(client_sock, str, msglen) == -1) {
                DBG(printf("[WORKER %ld]: Sending result to the client failed: %s\n", pthread_self(), strerror(errno)));
            }

            // then read another string and loop
            msglen = read(client_sock, str, BUFSZ);
            if(msglen == -1) {
                DBG(printf("[WORKER %ld]: read() error. Exiting\n", pthread_self()));
                exit(-1);
            }
        }

        // the client sent the string "quit", so the thread frees resources and terminates
        free(str);
        // close the socket used for the connection
        close(client_sock);

        // give feedback to the server
    }
    return (void*)0;
}
