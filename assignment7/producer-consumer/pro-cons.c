/*
 * pro-cons.c
 * 
 * Copyright 2021 nicola vetrini <nicola@ubuntu>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

/* Testo:
 * Scrivere un programma C con due threads: un produttore (P) ed un consumatore (C).
 * Il thread P genera, uno alla volta, una sequenza di numeri inserendoli in un
 * buffer di una sola posizione condiviso con il thread C. Il thread consumatore
 * estrae i numeri dal buffer e li stampa sullo standard output. Se il buffer e'
 * pieno P attende che C consumi il dato, analogamente se il buffer e' vuoto C
 * attende che P produca un valore da consumare.
 */
 
// pthreads lib headers
#include <pthread.h>
// unix syscalls
#include <unistd.h>
// std lib headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

// conditional compilation to wipe debug statements when the flag DEBUG is omitted
#if defined(DEBUG)
#define DBG(X) X
#else
#define DBG(X)
#endif 

// rand_r is not part of c99
int rand_r(unsigned int *seedp);

// producer function
void *producer(void *arg);
// consumer function
void *consumer(void *arg);

// 1-position buffer
static unsigned int buf;
// states the buffer can have are defined below, as well as the (shared) variable holding
// the buffer's state
#define FULL 1
#define EMPTY 0
#define CLOSED -1
static int bufState = FULL;

// a lock controls mutual exclusion access to buf
pthread_mutex_t lock_buf = PTHREAD_MUTEX_INITIALIZER;
// a condition variable to control changes on buf
pthread_cond_t buf_full = PTHREAD_COND_INITIALIZER;

// The program takes an optional arg indicating the number of writes to be done
int main(int argc, char **argv)
{
	int nwrites = -1;
	// the program takes an optional arg
	if(argc == 2 && argc < 3) {
		nwrites = atoi(argv[1]);
		assert(nwrites > 0);
	}
	
	// creates the consumer thread
	pthread_t consumer_tid;
	int ret;
	if((ret = pthread_create(&consumer_tid, NULL, consumer, NULL)) != 0) {
		errno = ret;
		perror("Consumer thread cannot be created");
		return 1;
	}


	// creates the producer thread
	pthread_t producer_tid;
	if((ret = pthread_create(&producer_tid, NULL, producer, (void*)nwrites)) != 0) {
		errno = ret;
		perror("Producer thread cannot be created");
		return 2;
	}

	int thread_ret;
	// wait for the producer's termination
	if((ret = pthread_join(producer_tid, (void*)&thread_ret)) != 0) {
		errno = ret;
		perror("Thread cannot be joined");
		return 3;
	}
	printf("Producer thread %lu returned %d\n", producer_tid, thread_ret);

	if((ret = pthread_join(consumer_tid, (void*)thread_ret)) != 0) {
		errno = ret;
		perror("Consumer thread cannot be joined");
		return 3;
	}
	printf("Consumer thread %lu returned %d\n", consumer_tid, thread_ret);
	
	return 0;
}

// the consumer thread's function
void *consumer(void *arg) {
	
	int nread = 0;
	int term = 0; // flag to terminate the reader thread the buffer is closed
	// the loop is executed until termination is signaled by the writer
	while(!term) {
		// get the lock first
		int ret;
		if((ret = pthread_mutex_lock(&lock_buf)) == -1) {
			errno = ret;
			perror("Consumer cannot lock buf");
		}
		
		// now the thread is in mutual exclusion
		// if the buffer is empty, wait until the producer generates some value
		while(bufState == EMPTY) {
			// release the lock as well
			pthread_cond_wait(&buf_full, &lock_buf);
		}
		// if the buffer is closed, exit the reading loop
		if(bufState == CLOSED) {
			term = 1;
		}
		// else the buffer is FULL, so print the data and set it to empty
		else {
			nread++; // add 1 to the number of values read
			DBG(printf("Consumer: BUF is %u\n", buf));
			
			bufState = EMPTY;
			// signal waiting producer(s) the state change
			pthread_cond_signal(&buf_full);
		}

		
		if((ret = pthread_mutex_unlock(&lock_buf)) == -1) {
			errno = ret;
			perror("Consumer cannot unlock buf");
		}
		
	}

	DBG(printf("Consumer terminates\n"));

	// returns the number of values read
	return (void*)nread;
}

// the producer thread's function
void *producer(void *arg) {
	// unpack arguments
	int n = (int)arg;
	
	unsigned int seed = time(NULL); // rand seed
	int ret;
	
	// do exactly n writes or infinite loop if n is -1
	int i = 0;
	int finite = 0;
	if(n != -1) finite = 1;
	
	while(!finite || i < n) {
		if(finite) i++;
		// get the lock first
		if((ret = pthread_mutex_lock(&lock_buf)) == -1) {
			errno = ret;
			perror("Producer cannot lock buf");
		}

		// if the buffer is not empty: wait until the consumer consumes its content
		while(bufState == FULL) {
			// release the lock as well
			pthread_cond_wait(&buf_full, &lock_buf);
		}
		// now the buffer is EMPTY, so generate a new value using rand_r
		buf = rand_r(&seed);

		// debug print outputs the value just written to buf
		DBG(printf("Producer: BUF set to %u\n", buf));

		// now the buffer is FULL
		bufState = FULL;
		// signal waiting consumer(s) the state change
		pthread_cond_signal(&buf_full);

		// release the lock
		if((ret = pthread_mutex_unlock(&lock_buf)) == -1) {
			errno = ret;
			perror("Producer cannot unlock buf");
		}
	}

	// take mutual exclusion yet again
	// get the lock first
	if((ret = pthread_mutex_lock(&lock_buf)) == -1) {
		errno = ret;
		perror("Producer cannot lock buf");
	}

	// the writer will not put any more values => close the buffer
	bufState = CLOSED;
	pthread_cond_signal(&buf_full);
	
	// release the lock
	if((ret = pthread_mutex_unlock(&lock_buf)) == -1) {
		errno = ret;
		perror("Producer cannot unlock buf");
	}

	DBG(printf("Producer terminates\n"));

	// return the number of values produced
	return (void*)i;
}
