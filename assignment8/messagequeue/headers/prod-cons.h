// header for the producer/consumer problem

#ifndef PROD_CONS_H
#define PROD_CONS_H

// my headers
#include <fifo-queue.h>
// threads library
#include <pthread.h>

// macro to strip debug prints when compiled with -DNDEBUG
#ifdef DEBUG
#define DBG(X) (X)
#else
#define DBG(X)
#endif

// declare global queue for communications
extern struct Queue *q_head;
extern struct Queue *q_tail;

// syncronization variables for the queue
extern pthread_mutex_t q_mux;
extern pthread_cond_t q_newmsg;

// functions executed by threads
void *send_msg(void *nmess);
void *consume(void *unused);

#endif
