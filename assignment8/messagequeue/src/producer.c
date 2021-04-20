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

int rand_r(unsigned int *seedp);

int send(void *msg, size_t sz);

void *send_msg(void *nmess) {
    long max_mess = (long)nmess; // brutal casting
    DBG(printf("%ld: Will send %ld messages\n", pthread_self(), max_mess));
    
    // the message is just a pseudo-random integer
    unsigned int seed = time(NULL);
    long x;
    size_t len = sizeof(x);
    for(long i  = 0; i < max_mess; i++) {
        x = 1 + rand_r(&seed);
        if(send((void*)x, len) == -1) {
            return (void*)-1;
        }
    }
    // no more messages to send, exit
    return (void*)0;
}

int send(void *msg, size_t sz) {
    int ret;
    if((ret = pthread_mutex_lock(&q_mux)) == -1) {
        printf("%ld: cannot lock queue: %s\n", pthread_self(), strerror(ret));
        return -1;
    }
    
    // operating in ME on the queue
    if(enqueue(&q_head, &q_tail, msg, sz) == NULL) {
        printf("%ld: cannot enqueue\n", pthread_self());
    }
    // signal any waiting consumer that a new message has been deposited
    pthread_cond_signal(&q_newmsg);
    
    if((ret = pthread_mutex_unlock(&q_mux)) == -1) {
        printf("%ld: cannot unlock queue: %s\n", pthread_self(), strerror(ret));
        return -1;
    }
    return 0;
}
