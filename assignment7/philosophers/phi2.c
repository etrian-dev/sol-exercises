/*
 * dining-philosopher2.c
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

// pthread header
#include <pthread.h>
// std lib headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <time.h>

// macro to strip debugging prints when not needed
#if defined(DEBUG)
#define DBG(X) (X)
#else
#define DBG(X)
#endif

// This program solves the dining philosophers problem by grabbing a fork in ME, then
// try grabbing the other. If it succeds, then eat. Otherwise, release the held fork
// then wait and then swap the order and try again

// Each fork can be held by at most one philosopher at a time,
// so each one must have its own mutex
pthread_mutex_t *mux_forks;

static long n;
static long iter;
static int *forks;

// code executed by the philosopher
void *phi(void *position);
// code to grab in ME forks nearby philosopher at position pos
void trygrab_forks(long pos);
// code to release ME on grabbed forks
void release_forks(long pos);

// This program solves the dining philosophers by using active waiting
// The program takes two parameters: the number of philosophers (and forks) and
// the amount of times the philosoher (thread) eats before terminating

// Each fork can be held by at most one philosopher at a time,
// so each one must have its own mutex

int main(int argc, char **argv)
{
	if(argc != 3) {
		DBG(printf("Usage: %s < #philosophers > < #dinners >\n", argv[0]));
	}
	else {
		// assuming no conversion errors
		long nPhil = strtol(argv[1], NULL, 10);
		long nIter = strtol(argv[2], NULL, 10);
		assert(nPhil > 1);
		assert(nIter > 0);

		// state the execution parameters
		printf("Number of philosophers: %ld\n", nPhil);
		printf("Number of dinners per philosopher: %ld\n", nIter);

		// alloc forks for the philosophers
		forks = malloc(nPhil * sizeof(int));
		if(!forks) {
			perror("malloc forks");
			return 1;
		}

		// alloc mutexes on forks
		mux_forks = malloc(nPhil * sizeof(pthread_mutex_t));
		if(!mux_forks) {
			perror("malloc mutex variables");
			return 1;
		}

		
		// alloc memory to hold thread IDs
		pthread_t *philosophers = malloc(nPhil * sizeof(pthread_t));
		if(!philosophers) {
			perror("malloc philosophers");
			return 1;
		}
		
		// initialize mutexes on forks and set all forks to free
		long i;
		for(i = 0; i < nPhil; i++) {
			// the second param to mutex_init is NULL, so the default mutex type is created
			if(pthread_mutex_init(&mux_forks[i], NULL) != 0) {
				perror("Initializing mutex");
				return 2;
			}
			// fork i is free at the beginning
			forks[i] = 1;
		}
		
		// initialize the number of philosophers and the number of iterations
		n = nPhil;
		iter = nIter;

		int retval;
		for(i = 0; i < nPhil; i++) {
			// the index of the thread into the array is passed, so that each thread
			// can determine the state of adjacent forks
			if((retval = pthread_create(philosophers + i, NULL, &phi, (void*)i)) == -1) {
				perror("Cannot create philosopher");
				return 3;
			}
		}

		// now wait for all philosophers to terminate: they will do so when the philosopher
		// ate nIter times. The return value is just useless and thus ignored
		for(i = 0; i < nPhil; i++) {
			if((retval = pthread_join(philosophers[i], NULL)) == -1) {
				perror("Cannot join philosopher");
				return 4;
			}
		}

		// free everything & exit
		free(philosophers);
		free(forks);
		free(mux_forks);
	}
	return 0;
}

// A philosopher (thread) does two actions: meditate, then eat (or try to) and repeat
// Each philosopher observes an ordering in acquiring forks,
// so that no deadlock can occurr
void *phi(void *position) {
	// to add a bit of pseudo-randomness to eating and meditation
	struct timespec delay = {0, 0};
	unsigned seed = time(NULL);
	
	// for easiness of conversion
	long pos = (long int)position;
	
	// Each philosopher eats exactly iter times
	for(long int i = 0; i < iter; i++) {
		DBG(printf("Philosopher %ld meditates\n", pos));
		// meditate first: sleeps for a random-ish amount of nanoseconds
		delay.tv_nsec = rand_r(&seed) % 999999999;
		nanosleep(&delay, NULL);
		
		// the code is not symmetric: philosophers at even positions try to get hold of
		// fork i, then i+1. Philosophers at odd positions do the opposite
		// This way, no deadlock occurs.
		trygrab_forks(pos);
		DBG(printf("Philosopher %ld gets forks\n", pos));

		DBG(printf("Philosopher %ld eats\n", pos));
		// spend some random time eating
		delay.tv_nsec = rand_r(&seed) % 999999999;
		nanosleep(&delay, NULL);

		// release forks. First releases the first grabbed fork, then the other one
		release_forks(pos);
		DBG(printf("Philosopher %ld released forks\n", pos));
	}
	DBG(printf("Philosopher %ld terminated!\n", pos));
	return NULL;
}

void trygrab_forks(long pos) {
	int retval;
	// to add a bit of pseudo-randomness to eating and meditation
	struct timespec delay = {0, 0};
	unsigned seed = time(NULL);

	int firstfork = pos;
	int secondfork = (pos + 1) % n;

	// grab the first
	if((retval = pthread_mutex_lock(&mux_forks[firstfork])) == -1) {
		perror("Cannot lock first fork");
	}

	DBG(printf("Philosopher %ld: fork %ld acquired\n", pos, pos));

	// try grabbing the second
	while(pthread_mutex_trylock(&mux_forks[secondfork]) == EBUSY) {
		DBG(printf("Philosopher %ld: fork %ld is BUSY\n", pos, (pos + 1) % n));
		// release fork pos
		if((retval = pthread_mutex_unlock(&mux_forks[firstfork])) == -1) {
			perror("Cannot unlock first fork");
		}
		// wait, then swap the order
		delay.tv_nsec = rand_r(&seed) % 999999999;
		nanosleep(&delay, NULL);

		int tmp = firstfork;
		firstfork = secondfork;
		secondfork = tmp;
	}
	
	DBG(printf("Philosopher %ld: fork %ld acquired\n", pos, (pos + 1) % n));
	// now secondfork is surely in ME, so both can be set to BUSY safely
	forks[firstfork] = 0;
	forks[secondfork] = 0;
}
// simply release both
void release_forks(long pos) {
	int retval;

	forks[pos] = 1;

	if((retval = pthread_mutex_unlock(&mux_forks[pos])) == -1) {
		perror("Cannot unlock first fork");
	}

	forks[(pos + 1) % n] = 1;

	if((retval = pthread_mutex_unlock(&mux_forks[(pos + 1) % n])) == -1) {
		perror("Cannot unlock second fork");
	}
}

