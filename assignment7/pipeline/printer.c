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
	// stream termination flag, set by the special token {NULL, NULL}
	int end_of_stream = 0;

	while (!end_of_stream)
	{
		int ret;
		// take mutex on token stream
		if ((ret = pthread_mutex_lock(&mux_tokbuf)) == -1)
		{
			perror("lock line buffer");
			exit(-1);
		}

		// wait until there are no tokens in the queue
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
			// free the last token in the stream (it's malloc's at line-reader.c:66)
			free(elem);
		}
		else
		{
			// print the token to stdout
			if (strrchr(elem->data, '\n') == NULL)
			{
				fprintf(stdout, "%s ", elem->data);
			}
			else
			{
				fprintf(stdout, "%s", elem->data);
			}
			free(elem->data);
			free(elem);
		}
	}
	//DBG(printf("Printer thread terminated"); fflush(stdout));

	return NULL;
}