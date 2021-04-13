/*
 * tokenizer-pipeline.c
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
// unix headers for nanosleep
#include <unistd.h>
// std lib headers
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

// declare global queues for communications
struct Queue *q_lines_head = NULL;
struct Queue *q_lines_tail = NULL;
struct Queue *q_tokens_head = NULL;
struct Queue *q_tokens_tail = NULL;

// syncronization variables for the buffers
pthread_mutex_t mux_lnbuf = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mux_tokbuf = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t lnbuf_new = PTHREAD_COND_INITIALIZER;
pthread_cond_t tokbuf_new = PTHREAD_COND_INITIALIZER;

int main(int argc, char **argv)
{
	// A filename must be passed as argument
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
	}
	else
	{
		// Open the file (and report the error and exit otherwise)
		FILE *fp;
		if ((fp = fopen(argv[1], "r")) == NULL)
		{
			perror("Cannot open input file");
			return 1;
		}
		// the threads must then be created
		pthread_t reader, tokenizer, printer;
		int ret;
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

		int lines;
		// try to join all the threads
		if((ret = pthread_join(reader, (void *)&lines)) != 0) {
			printf("Cannot join thread %lu: %s\n", reader, strerror(ret));
		}
		// all went well
		// the file can be closed
		if (fclose(fp) == -1)
		{
			perror("File can't be closed");
			return 5;
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
