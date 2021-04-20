// my headers
#include <fifo-queue.h>
#include <prod-cons.h>
// threads library
#include <pthread.h>
// std lib headers
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

void *consume(void *unused) {
    // until the termination message is received, get a message
    int term = 0;
    
    while(!term) {
        // get ME on the queue
        int ret;
        if((ret = pthread_mutex_lock(&q_mux)) == -1) {
            printf("%ld: cannot lock queue: %s\n", pthread_self(), strerror(ret));
            return (void*)-1;
        }
        
        // wait until there are messages in the queue
        struct Queue *msg;
        while((msg = pop(&q_head, &q_tail)) == NULL) {
            pthread_cond_wait(&q_newmsg, &q_mux);
        }
        // A new message received: examine it to determine if it signals stream termination
        if(msg->data == NULL && msg->next == NULL) {
            term = 1; // terminates on the next guard test
            // the message is reinserted in the queue and other consumers are signaled
            if(!q_tail) {
                q_tail = q_head = msg;
            }
            else {
                q_tail->next = msg;
                q_tail = msg;
            }
            pthread_cond_signal(&q_newmsg);
        }
        else {
            // just a regular message: print it (it's a long int)
            printf("%ld: Message received: %ld\n", pthread_self(), (long)(*(msg->data)));
            // then free it
            free(msg->data);
            free(msg);
        }
        
        if((ret = pthread_mutex_unlock(&q_mux)) == -1) {
            printf("%ld: cannot unlock queue: %s\n", pthread_self(), strerror(ret));
            return (void*)-1;
        }
    }
    return (void*)0;
}

