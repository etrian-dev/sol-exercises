/*
 * Developer: nicola vetrini (nicola@ubuntu)
 * Created: 2021-04-13
 * Version: 1.0
 * phi3.c
 */


// threading library header
#include <pthread.h>
// std unix headers

// std lib headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>

// macro to strip debugging prints when not needed
#if defined(DEBUG)
#define DBG(X) (X)
#else
#define DBG(X)
#endif

/* This solution of the dining philosophers problem uses monitors: each philosopher
 * knows what state other philosophers on the table are in. There are three possible states:
 * Hungry (requested for forks, but hasn't obtained them yet), Thinking or Eating
 * (is holding both forks). When a philosopher finishes eating, he tries to help his
 * neighbours by giving them his forks (if they were waiting for them). Helping
 */

// defines the State enum
typedef enum State
{
	THINKING,
	HUNGRY,
	EATING
} State;

// the state array is shared by all philosophers, so it must have its own lock as well
pthread_mutex_t mux_state = PTHREAD_MUTEX_INITIALIZER;
// each philosopher waits on its own cond variable
pthread_cond_t *cond_phi;

// number of philosophers
static long n;
// number of iterations each philosopher completes before terminating (# of dinners)
static long iter;
// the array of states
static int *phi_state;
// the array of forks
static int *forks;
#define FREE 1
#define BUSY 0

// code executed by the philosopher
void *phi(void *position);
// code to grab in ME forks nearby philosopher at position pos
void grab_forks(long pos);
// code to release ME on grabbed forks
void release_forks(long pos);

// Launch the program and read arguments,
// then create threads and wait for their termination
int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Usage: %s < #philosophers > < #dinners >\n", argv[0]);
	}
	else
	{
		// assuming no conversion errors
		long nPhil = strtol(argv[1], NULL, 10);
		long nIter = strtol(argv[2], NULL, 10);
		assert(nPhil > 1);
		assert(nIter > 0);

		// state the execution parameters
		printf("Number of philosophers: %ld\n", nPhil);
		printf("Number of dinners per philosopher: %ld\n", nIter);

		// alloc philosopher state array
		phi_state = malloc(nPhil * sizeof(int));
		if (!phi_state)
		{
			perror("malloc philosophers'state array");
			return 1;
		}

		// alloc cond variables for each philosopher
		cond_phi = malloc(nPhil * sizeof(pthread_cond_t));
		if(!cond_phi) {
			perror("malloc cond variables");
			return 1;
		}

		// alloc forks
		forks = malloc(nPhil * sizeof(pthread_cond_t));
		if(!forks) {
			perror("malloc forks");
			return 1;
		}

		// alloc memory to hold thread IDs
		pthread_t *philosophers = malloc(nPhil * sizeof(pthread_t));
		if (!philosophers)
		{
			perror("malloc philosophers");
			return 1;
		}

		// initialize mutexes on forks and set all forks to free
		long i;
		for (i = 0; i < nPhil; i++)
		{
			// set all philosophers to thinking at first
			phi_state[i] = THINKING;
			forks[i] = FREE; // set fork to free
			// initialize their cond variables
			pthread_cond_init(&cond_phi[i], NULL);
		}

		// initialize the number of philosophers and the number of iterations
		n = nPhil;
		iter = nIter;

		int retval;
		for (i = 0; i < nPhil; i++)
		{
			// the index of the thread into the array is passed, so that each thread
			// can determine the state of adjacent forks
			if ((retval = pthread_create(philosophers + i, NULL, &phi, (void *)i)) == -1)
			{
				perror("Cannot create philosopher");
				return 3;
			}
		}

		// now wait for all philosophers to terminate: they will do so when the philosopher
		// ate nIter times. The return value is just useless and thus ignored
		for (i = 0; i < nPhil; i++)
		{
			if ((retval = pthread_join(philosophers[i], NULL)) == -1)
			{
				perror("Cannot join philosopher");
				return 4;
			}
		}

		// free everything & exit
		free(philosophers);
		free(cond_phi);
		free(phi_state);
		free(forks);
	}
	return 0;
}

// A philosopher (thread) does two actions: meditate, then eat (or try to) and repeat
// Each philosopher observes an ordering in acquiring forks,
// so that no deadlock can occurr
void *phi(void *position)
{
	// to add a bit of pseudo-randomness to eating and meditation
	struct timespec delay = {0, 0};
	unsigned seed = time(NULL);

	// for easiness of conversion
	long pos = (long int)position;

	// Each philosopher eats exactly iter times
	for (long int i = 0; i < iter; i++)
	{
		DBG(printf("Philosopher %ld meditates...\n", pos));

		// meditate first: sleeps for a random-ish amount of nanoseconds
		delay.tv_nsec = rand_r(&seed) % 999999999;
		nanosleep(&delay, NULL);

		// grab both forks in ME
		grab_forks(pos);

		// spend some random-ish time eating
		DBG(printf("Philosopher %ld eats...\n", pos));
		delay.tv_nsec = rand_r(&seed) % 999999999;
		nanosleep(&delay, NULL);
		DBG(printf("Philosopher %ld finished eating...\n", pos));

		// release both forks and try to help neighbours eating
		release_forks(pos);
	}
	DBG(printf("Philosopher %ld terminated!\n", pos));
	return NULL;
}

void grab_forks(long pos)
{
	int retval;

	// take mutex on state array to set the state to hungry
	if ((retval = pthread_mutex_lock(&mux_state)) == -1)
	{
		perror("Cannot lock state");
	}

	phi_state[pos] = HUNGRY;

	// this ugly expression because C does not define modulo operators, but reminder
	// so a % b does not work as expected if a < 0
	long int modulo_pos_min_1 = (pos == 0 ? n - 1 : (pos - 1) % n);

	// wait for BOTH neighbouring philosophers to finish eating
	while (phi_state[modulo_pos_min_1] == EATING || phi_state[(pos + 1) % n] == EATING)
	{
		DBG(printf("Philosopher %ld waits: %ld or %ld are eating\n", pos, modulo_pos_min_1, (pos + 1) % n));
		pthread_cond_wait(&cond_phi[pos], &mux_state); // give up mutex on state array
	}
	// now both neighbours are not eating, so set philosopher's state to EATING and grab forks
	phi_state[pos] = EATING;
	forks[pos] = BUSY;
	forks[(pos + 1) % n] = BUSY;

	if ((retval = pthread_mutex_unlock(&mux_state)) == -1)
	{
		perror("Cannot unlock state");
	}
}

// upon releasing forks determine if they can be given to some neighbouring philosopher
// if so, signal, otherwise just leave them free
void release_forks(long pos)
{
	int retval;

	// take mutex on state array to set the state THINKING
	if ((retval = pthread_mutex_lock(&mux_state)) == -1)
	{
		perror("Cannot lock state");
	}

	phi_state[pos] = THINKING;
	forks[pos] = FREE;
	forks[(pos + 1) % n] = FREE;

	// this ugly expression because C does not define modulo operators, but reminder
	// so a % b does not work as expected if a < 0
	long int modulo_pos_min_1 = (pos == 0 ? n - 1 : (pos - 1) % n);

	// if a neighbour is hungry, then pass them the fork
	if (phi_state[modulo_pos_min_1] == HUNGRY)
	{
		DBG(printf("Pass fork to %ld\n", modulo_pos_min_1));
		pthread_cond_signal(&cond_phi[modulo_pos_min_1]);
	}
	if (phi_state[(pos + 1) % n] == HUNGRY)
	{
		DBG(printf("Pass fork to %ld\n", (pos + 1) % n));
		pthread_cond_signal(&cond_phi[(pos + 1) % n]);
	}

	if ((retval = pthread_mutex_unlock(&mux_state)) == -1)
	{
		perror("Cannot unlock state");
	}
}
