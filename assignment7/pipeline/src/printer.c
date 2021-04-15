// my headers
#include <fifo-queue.h>
#include <pipeline.h>
// std headers
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

void *print_line(void *unused)
{
	int end_of_stream = 0; // stream termination flag, set by the special token {NULL, NULL}

	while (!end_of_stream)
	{
		int ret;
		// take mutex on token stream
		if ((ret = pthread_mutex_lock(&mux_tokbuf)) == -1)
		{
			perror("lock line buffer");
			exit(-1);
		}

		// wait until there are tokens in the queue
		struct Queue *elem;
		while ((elem = pop(&q_tokens_head, &q_tokens_tail)) == NULL)
		{
			pthread_cond_wait(&tokbuf_new, &mux_tokbuf);
		}

		// release mutex
		if ((ret = pthread_mutex_unlock(&mux_tokbuf)) == -1)
		{
			perror("unlock line buffer");
			exit(-1);
		}

		// if the token is {NULL, NULL} => terminate the thread
		if (elem->data == NULL && elem->next == NULL)
		{
			end_of_stream = 1;
			
			// free the last token in the stream
			// No need to free elem->data as no string is malloc'd (see code in line-reader.c)
			free(elem);
		}
		else
		{
			// print the token to stdout. If it contains a newline,
			// then no ' ' is added (otherwise the output of the next token will begin
			// on a new line, prefixed by a blank). 
			if (strrchr(elem->data, '\n') != NULL)
			{
				printf("%s", elem->data);
			}
			// Otherwise, add a blank to separate tokens on the same line
			else
			{
				printf("%s ", elem->data);
			}

			// free the token string and its struct
			free(elem->data);
			free(elem);
		}
	}

	DBG(printf("Printer thread terminated"); fflush(stdout));

	return NULL;
}
