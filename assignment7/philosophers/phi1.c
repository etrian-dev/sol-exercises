/*
 * dining-philosophers.c
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

// This program solves the dining philosophers
// given an arbitrary number of philosophers as the parameter

// The solution uses ordering to ensure no deadlock ensues

// pthread header
#include <pthread.h>
// unix headers
#include <unistd.h>
// std lib headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

void *phi(void *position);
void grab_forks(long pos);
void release_forks(long pos);

pthread_mutex_t *mux_forks;

static long n;
static long iter;
static int *forks;

int main(int argc, char **argv)
{
	if(argc != 3) {
		printf("Usage: %s < #philosophers > < #iterations >\n", argv[0]);
	}
	else {
		// assuming no conversion errors
		long nPhil = strtol(argv[1], NULL, 10);
		long nIter = strtol(argv[2], NULL, 10);
		assert(nPhil > 0);
		assert(nIter > 0);

		long i;

		// initialize mutex variables
		mux_forks = malloc(nPhil * sizeof(pthread_mutex_t));
		if(!mux_forks) {
			perror("malloc mutex variables");
			return 1;
		}

		// alloc forks for the philosophers
		forks = malloc(nPhil * sizeof(int));
		if(!forks) {
			perror("malloc forks");
			return 1;
		}
		
		// alloc memory to hold thread IDs of the given amount of threads
		pthread_t *philosophers = malloc(nPhil * sizeof(pthread_t));
		if(!philosophers) {
			perror("malloc philosophers");
			return 1;
		}
		
		// initialize mutexes on forks and set all forks to free
		// no thread has yet been created, so it's safe to do so without using mutexes
		for(i = 0; i < nPhil; i++) {
			pthread_mutex_init(&mux_forks[i], NULL);
			forks[i] = 1;
		}
		// initialize the number of philosophers and the number of iterations
		n = nPhil;
		iter = nIter;
		
		int retval;
		for(i = 0; i < nPhil; i++) {
			// since this solution uses ordering, each philosopher needs to know
			// his position in the table (his index in the array)
			if((retval = pthread_create(philosophers + i, NULL, &phi, (void*)&i)) == -1) {
				perror("Cannot create philosopher");
			}
		}
		
		long nEaten = 0; // this variable stores how many times philosopher i ate
		// This is the summary table header
		//printf("%11s\t%3s\n", "Philosopher", "Ate");
		
		// now wait for all philosophers to terminate after that many iterations
		for(i = 0; i < nPhil; i++) {
			if((retval = pthread_join(philosophers[i], (void*)&nEaten)) == -1) {
				perror("Cannot join philosopher");
			}
			// print the number of times philosopher i ate
			printf("%11ld\t%ld\n", i, nEaten);
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
	long pos = *(int*)position;
	int nEats = 0; // counts the number of times the thread executed the eat() function

	long i;
	for(i = 0; i < iter; i++) {
		// sleep first
		printf("Philosopher %ld sleeps\n", pos);
		sleep(1);
		
		// the code is not symmetric: philosophers at even positions try to get hold of
		// fork i, then i+1. Philosophers at odd position try the opposite
		grab_forks(pos);
		printf("Philosopher %ld eats\n", pos);

		// spend some time eating
		sleep(1);
		nEats++;

		// release forks
		release_forks(pos);
		
		//sleep(1);
	}

	return (void*)nEats;
}

void grab_forks(long pos) {
	int retval;

	int firstfork = (pos % 2 == 0 ? pos : pos + 1 % n);
	int secondfork = (pos % 2 == 0 ? (pos + 1) % n : (pos - 1) % n);
	
	if((retval = pthread_mutex_lock(&mux_forks[firstfork])) == -1) {
		perror("Cannot lock first fork");
	}

	forks[firstfork] = 0;

	if((retval = pthread_mutex_lock(&mux_forks[secondfork])) == -1) {
		perror("Cannot lock second fork");
	}

	forks[secondfork] = 0;
}

void release_forks(long pos) {
	int retval;

	int firstfork = (pos % 2 == 0 ? pos + 1 % n : pos);
	int secondfork = (pos % 2 == 0 ? (pos - 1) % n : (pos + 1) % n);

	forks[firstfork] = 1;

	if((retval = pthread_mutex_unlock(&mux_forks[firstfork])) == -1) {
		perror("Cannot unlock first fork");
	}

	forks[secondfork] = 1;

	if((retval = pthread_mutex_unlock(&mux_forks[secondfork])) == -1) {
		perror("Cannot lock second fork");
	}
}
	
