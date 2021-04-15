/* TESTO
 * Scrivere un programma C che implementa una pipeline di tre threads.
 * Il primo thread legge una riga alla volta da un file testuale (il cui nome e' passato
 * come argomento al programma) ed invia al secondo thread ogni riga letta dal file.
 *
 * Il secondo thread “tokenizza” le parole dalla riga ricevuta dal primo thread
 * (considerare come separatore solo lo spazio) ed invia al terzo thread le parole.
 *
 * Il terzo thread stampa tutte le parole sullo standard output. I tre stadi della
 * pipeline devono lavorare in modo concorrente come in una “catena di montaggio”,
 * il buffer di comunicazione tra due stadi della pipeline deve essere implementata
 * con una coda FIFO (la scelta se usare una coda di capacita bounded o unbounded e'
 * lasciata allo studente).
 */

// my headers
#include <fifo-queue.h>
#include <pipeline.h>
// threads library
#include <pthread.h>
// std lib headers
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

// declare & initialize queues and sync variables here to avoid multiple definitions

// the lines queue is shared by the line reader thread and the tokenizer thread
// to send and receive lines
struct Queue *q_lines_head = NULL;
struct Queue *q_lines_tail = NULL;
// the tokens queue is shared by the tokenizer and the printer threads
// to send and receive tokens
struct Queue *q_tokens_head = NULL;
struct Queue *q_tokens_tail = NULL;

// mutexes for each queue
pthread_mutex_t mux_lnbuf = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mux_tokbuf = PTHREAD_MUTEX_INITIALIZER;
// condition variables for each queue are defined as well
pthread_cond_t lnbuf_new = PTHREAD_COND_INITIALIZER;
pthread_cond_t tokbuf_new = PTHREAD_COND_INITIALIZER;


// The main function reads args to the program, creates the threads and joins them, then exits
int main(int argc, char **argv)
{
	// A (valid) filename must be passed as argument
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
	}
	else
	{
		// Open the file in read mode (or report errors and exit otherwise)
		FILE *fp;
		if ((fp = fopen(argv[1], "r")) == NULL)
		{
			printf("[Main] cannot open input file %s: %s\n", argv[1], strerror(errno));
			return 1;
		}
		
		// the threads are then created
		pthread_t reader, tokenizer, printer;
		int ret;
		// the reader thread must know the file pointer as well, so that it can read the file
		if ((ret = pthread_create(&reader, NULL, &read_line, (void *)fp)) != 0)
		{
			printf("[Main] line reader thread cannot be created: %s\n", strerror(ret));
			return 2;
		}
		if ((ret = pthread_create(&tokenizer, NULL, &tokenize_line, NULL)) != 0)
		{
			printf("[Main] tokenizer thread cannot be created: %s\n", strerror(ret));
			return 3;
		}
		if ((ret = pthread_create(&printer, NULL, &print_line, NULL)) != 0)
		{
			printf("[Main] printer thread cannot be created: %s\n", strerror(ret));
			return 4;
		}

		// the main waits for all threads to terminate
		int lines;
		if((ret = pthread_join(reader, (void *)lines)) != 0) {
			printf("Cannot join thread %lu: %s\n", reader, strerror(ret));
		}
		// the file can be closed because the reader thread is terminated
		if (fclose(fp) == -1)
		{
			perror("File can't be closed");
			return 1;
		}
		
		if((ret = pthread_join(tokenizer, NULL)) != 0) {
			printf("Cannot join thread %lu: %s\n", tokenizer, strerror(ret));
		}
		
		if((ret = pthread_join(printer, NULL)) != 0) {
			printf("Cannot join thread %lu: %s\n", printer, strerror(ret));
		}
	}
	return 0;
}
