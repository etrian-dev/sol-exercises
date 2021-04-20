/* TESTO
 * Scrivere un programma C in cui si attivano M thread produttori ed N thread
 * consumatori che condividono una coda (di capacità “infinita”). Il programma
 * accetta come argomento anche un intero K che corrisponde al numero totale di
 * messaggi che i produttori producono concorrentemente nella coda
 * (ogni produttore produce K/M messaggi – se M divide K). I consumatori leggono
 * i messaggi dalla coda in un ciclo infinito fino a quando non ricevono un
 * messaggio speciale che li fa terminare. Implementare la coda concorrente, ed
 * il protocollo di terminazione senza usare la cancellazione esplicita dei threads.
 *
 * Testare il programma al variare di M ed N. Controllare con valgrind che non
 * ci siano problemi nell'accesso alle variabili del programma e che tutta la
 * memoria allocata sia completamente rilasciata all'uscita del programma.
 */

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
#include <assert.h>

// declare global queues for communications
struct Queue *q_head = NULL;
struct Queue *q_tail = NULL;

// syncronization variables for the buffers
pthread_mutex_t q_mux;
pthread_cond_t q_newmsg;

int main(int argc, char **argv)
{
    // An integer must be passed as an argument
    if (argc != 4)
    {
        printf("Usage: %s <K> <M> <N>\n", argv[0]);
    }
    else
    {
        // convert args to integers
        long tot_mess = strtol(argv[1], NULL, 10);
        long nprod = strtol(argv[2], NULL, 10);
        long ncons = strtol(argv[3], NULL, 10);

        assert(tot_mess > 0);
        assert(nprod > 0);
        assert(nprod <= tot_mess);
        assert(ncons > 0);

        // messages per producer: tot_mess / nprod
        long nmess = tot_mess / nprod;
        // if tot_mess % nprod != 0 the last producer produces the remainder
        long remainder = tot_mess - (nprod - 1) * nmess;

        // alloc array of threads
        pthread_t *producers = malloc(nprod * sizeof(pthread_t));
        if(!producers) {
            perror("Cannot alloc producers");
            return 1;
        }
        pthread_t * consumers = malloc(ncons * sizeof(pthread_t));
        if(!consumers) {
            perror("Cannot alloc consumers");
            return 1;
        }
        
        // initialize mutexes and cond variables
        pthread_mutex_init(&q_mux, NULL);
        pthread_cond_init(&q_newmsg, NULL);        
                
        // create producers threads
        int i, ret;
        for(i = 0; i < nprod - 1; i++) {
            if ((ret = pthread_create(&producers[i], NULL, &send_msg, (void*)nmess)) != 0) {
                errno = ret;
                printf("%s: Producer thread can't be created: %s\n", argv[0], strerror(ret));
                return 2;
            }
        }
        // the last thread may produce less than nmess
        if ((ret = pthread_create(&producers[nprod-1], NULL, send_msg, (void*)remainder)) != 0) {
            errno = ret;
            printf("%s: Producer thread can't be created: %s\n", argv[0], strerror(ret));
            return 2;
        }
        
        // create consumers threads
        for(i = 0; i < ncons; i++) {
            if ((ret = pthread_create(&consumers[i], NULL, consume, NULL)) != 0) {
                errno = ret;
                printf("%s: Consumer thread can't be created: %s\n", argv[0], strerror(ret));
                return 2;
            }
        }

        // try to join all the threads
        // join producers first
        for(i = 0; i < nprod; i++) {
            if((ret = pthread_join(producers[i], NULL)) != 0) {
                errno = ret;
                printf("%s: Cannot join thread %ld: %s\n", argv[0], producers[i], strerror(ret));
                return 3;
            }
        }
        
        // Several special termination messages are put in the queue, consisting of
        // {NULL, NULL}. Exactly ncons copies must be inserted, since all consumers receiving it
        // will just terminate.
        struct Queue *EOstream = malloc(sizeof(struct Queue));
        if(!EOstream) {
            printf("%ld: cannot alloc termination messages: %s\n", pthread_self(), strerror(ret));
        } // the messages are already initialized, since calloc zeroes memory
        EOstream->data = NULL;
        EOstream->next = NULL;
        
        // get ME on the queue to insert the termination tokens
        if((ret = pthread_mutex_lock(&q_mux)) == -1) {
            printf("%ld: Main thread cannot lock queue: %s\n", pthread_self(), strerror(ret));
        }
        
        if(!q_tail) {
            q_tail = q_head = EOstream;
        }
        else {
            q_tail->next = EOstream;
            q_tail = EOstream;
        }
        // then broadcast all consumers to make sure they receive it
        pthread_cond_signal(&q_newmsg);
        
        if((ret = pthread_mutex_unlock(&q_mux)) == -1) {
            printf("%ld: Main thread cannot unlock queue: %s\n", pthread_self(), strerror(ret));
        }
        
        // try to join all consumers
        for(i = 0; i < ncons; i++) {
            if((ret = pthread_join(consumers[i], NULL)) != 0) {
                errno = ret;
                printf("%s: Cannot join thread %ld: %s\n", argv[0], consumers[i], strerror(ret));
                return 3;
            }
        }
        
        // then remove the termination message from the queue
        // no other thread is executing, so no need to be in ME
        EOstream = pop(&q_head, &q_tail);
        free(EOstream);

        // free arrays of threads
        free(producers);
        free(consumers);
    }
    return 0;
}
