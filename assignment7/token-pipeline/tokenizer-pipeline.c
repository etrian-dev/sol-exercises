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

// my library implementing a FIFO queue
#include <fifo-queue.h>
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

#ifdef DEBUG
#define DBG(X) X
#else
#define DBG(X)
#endif

// defining them because c99 complains
char *strdup(const char *s);
char *strndup(const char *s, size_t sz);
size_t strnlen(const char *s, size_t maxlen);

// functions executed by each thread
void *read_line(void *file_desc);
void *tokenize_line(void *unused);
void *print_line(void *unused);

// declare global queues for communications
struct Queue *q_lines_head = NULL, *q_lines_tail = NULL;
struct Queue *q_tokens_head = NULL, *q_tokens_tail = NULL;

// syncronization variables for the buffers
pthread_mutex_t mux_lnbuf = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mux_tokbuf = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t lnbuf_new = PTHREAD_COND_INITIALIZER;
pthread_cond_t tokbuf_new = PTHREAD_COND_INITIALIZER;

int main(int argc, char **argv)
{
	// A filename must be passed as argument
	if(argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
	}
	else {
		// Open the file (and report the error and exit otherwise)
		FILE *fp;
		if((fp = fopen(argv[1], "r")) == NULL) {
			perror("Cannot open input file");
			return 1;
		}
		// the threads must then be created
		pthread_t reader, tokenizer, printer;
		int ret;
		if((ret = pthread_create(&reader, NULL, &read_line, (void*)fp)) == -1) {
			perror("Reader thread cannot be created");
			return 2;
		}
		if((ret = pthread_create(&tokenizer, NULL, &tokenize_line, NULL)) == -1) {
			perror("Tokenizer thread cannot be created");
			return 3;
		}
		if((ret = pthread_create(&printer, NULL, &print_line, NULL)) == -1) {
			perror("Printer thread cannot be created");
			return 4;
		}

		int lines;
		// try to join all the threads
		pthread_join(reader, (void*)&lines);
		pthread_join(tokenizer, NULL);
		pthread_join(printer, NULL);

		//DBG(printf("Read %d lines from file %s\n", lines, argv[1]));

		// all went well
		// the file can be closed
		if(fclose(fp) == -1) {
			perror("File can't be closed");
			return 5;
		}
	}
	return 0;
}

#define MAX_LN_SZ 1000
// This thread receives the file descriptor as input and reads it line by line
void *read_line(void *file_pointer) {
	FILE *fpoint = (FILE*)file_pointer;

	// Now the line buffer can be alloc'd
	char *private_lnbuf = calloc(MAX_LN_SZ, sizeof(char));
	if(!private_lnbuf) {
		perror("reader: calloc private line buffer");
		return NULL;
	}

	int lines_read = 0;

	// read the file line by line inside buf
	while(fgets(private_lnbuf, MAX_LN_SZ, fpoint) != NULL) {
		lines_read++;

		// determine the lenght of the line just read
		size_t len = strnlen(private_lnbuf, MAX_LN_SZ);
		// replace the last \n with a terminator
		private_lnbuf[len-1] = '\0';

		// Take mutex on the queue
		int ret;
		if((ret = pthread_mutex_lock(&mux_lnbuf)) == -1) {
			perror("lock line buffer");
			exit(-1);
		}

		//DBG(printf("Read line: %s\n", private_lnbuf);fflush(stdout));
		
		// insert the line just read into the queue
		// NOTE: the string is duplicated internally, so it must be freed eventually
		enqueue(&q_lines_head, &q_lines_tail, private_lnbuf, len);
		// signal waiting tokenizers that at least one value is in the queue
		pthread_cond_signal(&lnbuf_new);

		// release mutex
		if((ret = pthread_mutex_unlock(&mux_lnbuf)) == -1) {
			perror("unlock line buffer");
			exit(-1);
		}
	}

	// to signal the end of the stream, add one last queue element, consisting of
	// {NULL, NULL}. The element is hand-crafted to avoid overcomplicating enqueue()
	struct Queue *EOstream = malloc(sizeof(struct Queue));
	if(!EOstream) {
		perror("cannot create end of stream token");
		exit(-1);
	}
	EOstream->data = malloc(21 * sizeof(char));
	memset(EOstream->data, (char)'1', 20 * sizeof(char));
	EOstream->data[20] = '\0';
	EOstream->next = NULL;

	int ret;
	if((ret = pthread_mutex_lock(&mux_lnbuf)) == -1) {
		perror("lock line buffer");
		exit(-1);
	}

	// insert the end of stream token
	if(q_lines_tail) {
		q_lines_tail->next = EOstream;
		q_lines_tail = EOstream;
	}
	else {
		q_lines_head = q_lines_tail = EOstream;
	}

	pthread_cond_signal(&lnbuf_new);
		
	// release mutex
	if((ret = pthread_mutex_unlock(&mux_lnbuf)) == -1) {
		perror("unlock line buffer");
		exit(-1);
	}
	
	// last thing: free the internal line buffer
	free(private_lnbuf);

	// if all went well then return the number of lines read
	return (void*)lines_read;
}
void *tokenize_line(void *unused) {
	// stream termination flag, set by a special element in the queue
	int end_of_stream = 0;
	while(!end_of_stream) {
	
		// Take mutex on the lines queue
		int ret;
		if((ret = pthread_mutex_lock(&mux_lnbuf)) == -1) {
			perror("lock line buffer");
			exit(-1);
		}
		
		char *str;
		while((str = pop(&q_lines_head, &q_lines_tail)) == NULL) {
			pthread_cond_wait(&lnbuf_new, &mux_lnbuf);
		}

		// release mutex on the line queue
		if((ret = pthread_mutex_unlock(&mux_lnbuf)) == -1) {
			perror("unlock line buffer");
			exit(-1);
		}
		
		if(strcmp(str, "11111111111111111111") == 0) {
			// special termination token: exit tokenizer and send termination to
			// the printer thread
			end_of_stream = 1;
			
			// {NULL, NULL}. The element is hand-crafted to avoid overcomplicating enqueue()
			struct Queue *EOstream = malloc(sizeof(struct Queue));
			if(!EOstream) {
				perror("cannot create end of stream token");
				exit(-1);
			}
			EOstream->data = malloc(21 * sizeof(char));
			memset(EOstream->data, (char)'1', 20 * sizeof(char));
			EOstream->data[20] = '\0';
			EOstream->next = NULL;
		
			int ret;
			if((ret = pthread_mutex_lock(&mux_tokbuf)) == -1) {
				perror("lock token buffer");
				exit(-1);
			}
		
			// insert the end of stream token
			if(q_tokens_tail) {
				q_tokens_tail->next = EOstream;
				q_tokens_tail = EOstream;
			}
			else {
				q_tokens_head = q_tokens_tail = EOstream;
			}
	
			pthread_cond_signal(&tokbuf_new);
				
			// release mutex
			if((ret = pthread_mutex_unlock(&mux_tokbuf)) == -1) {
				perror("unlock tokens buffer");
				exit(-1);
			}
		}
		else {
			//DBG(printf("Popped line: %s\n", str);fflush(stdout));
		
			// str is tokenized and its tokens will be added to the tokens queue (in ME)
			char *strtok_state = NULL;
			char *token = strtok_r(str, " ", &strtok_state);
			while(token != NULL) {
				// take mutex on the tokens queue
				if((ret = pthread_mutex_lock(&mux_tokbuf)) == -1) {
					perror("lock line buffer");
					exit(-1);
				}
		
				//DBG(printf("Token: %s\n", token);fflush(stdout));
		
				// enqueue the token on the queue
				enqueue(&q_tokens_head, &q_tokens_tail, token, strlen(token));
				// signal the printer thread of a new token
				pthread_cond_signal(&tokbuf_new);
		
				// release mutex
				if((ret = pthread_mutex_unlock(&mux_tokbuf)) == -1) {
					perror("unlock line buffer");
					exit(-1);
				}
				
				// get the next token
				token = strtok_r(NULL, " ", &strtok_state);
			}
		
			free(str);
		}
	}
	
	return NULL;
}
void *print_line(void *unused) {
	int ret;
	int end_of_stream = 0;

	while(!end_of_stream) {
		// take token mutex
		if((ret = pthread_mutex_lock(&mux_tokbuf)) == -1) {
			perror("lock line buffer");
			exit(-1);
		}
		
		char *str;
		while((str = pop(&q_tokens_head, &q_tokens_tail)) == NULL) {
			pthread_cond_wait(&tokbuf_new, &mux_tokbuf);
		}

		if(strcmp(str, "11111111111111111111") == 0) {
			end_of_stream = 1;
		}
		else {
			printf("%s ", str);
			fflush(stdout);
			free(str);
		}
		
		// release mutex
		if((ret = pthread_mutex_unlock(&mux_tokbuf)) == -1) {
			perror("unlock line buffer");
			exit(-1);
		}
	}
	return NULL;
}
