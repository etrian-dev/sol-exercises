// my headers
#include <fifo-queue.h>
#include <pipeline.h>
// std headers
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

void *tokenize_line(void *unused)
{
	int end_of_stream = 0; // stream termination flag, set by the special token {NULL, NULL}
	while (!end_of_stream)
	{
		int ret;
		// Take mutex on the lines queue
		if ((ret = pthread_mutex_lock(&mux_lnbuf)) == -1)
		{
			perror("lock line buffer");
			exit(-1);
		}

		// wait until there's at least one token in the queue
		struct Queue *elem;
		while ((elem = pop(&q_lines_head, &q_lines_tail)) == NULL)
		{
			pthread_cond_wait(&lnbuf_new, &mux_lnbuf);
		}

		// release mutex on the line queue
		if ((ret = pthread_mutex_unlock(&mux_lnbuf)) == -1)
		{
			perror("unlock line buffer");
			exit(-1);
		}

		// if the token is {NULL, NULL} => terminate the thread
		if (elem->data == NULL && elem->next == NULL)
		{
			end_of_stream = 1;

			// the termination token is forwarded to the tokens queue to terminate the printer as well
			if ((ret = pthread_mutex_lock(&mux_tokbuf)) == -1)
			{
				perror("lock token buffer");
				exit(-1);
			}

			// insert at the end of the token stream
			if (q_tokens_tail)
			{
				q_tokens_tail->next = elem;
				q_tokens_tail = elem;
			}
			else
			{
				q_tokens_head = q_tokens_tail = elem;
			}
			// signal as well
			pthread_cond_signal(&tokbuf_new);

			// release mutex
			if ((ret = pthread_mutex_unlock(&mux_tokbuf)) == -1)
			{
				perror("unlock tokens buffer");
				exit(-1);
			}
		}
		// a new line has been received: tokenize it and put all tokens in the tokens queue
		// to be received by the printer thread
		else
		{
			DBG(printf("Received line: %s\n", elem->data);fflush(stdout));

			// uses reentrant strtok, so its state is stored locally by the thread
			// and resetted to NULL each time a new line is being processed
			char *strtok_state = NULL;

			// tokens are strings separated by " "
			char *token = strtok_r(elem->data, " ", &strtok_state);
			while (token != NULL)
			{
				DBG(printf("Token: %s\n", token);fflush(stdout));

				// take mutex on the tokens queue
				if ((ret = pthread_mutex_lock(&mux_tokbuf)) == -1)
				{
					perror("lock line buffer");
					exit(-1);
				}

				// enqueue the token on the tokens queue
				enqueue(&q_tokens_head, &q_tokens_tail, token, strlen(token));

				// release mutex
				if ((ret = pthread_mutex_unlock(&mux_tokbuf)) == -1)
				{
					perror("unlock line buffer");
					exit(-1);
				}

				// get the next token
				token = strtok_r(NULL, " ", &strtok_state);
			}
			// signal the printer thread that a new tokens have been added
			pthread_cond_signal(&tokbuf_new);

			// free the processed line and its struct
			free(elem->data);
			free(elem);
		}
	}

	DBG(printf("Tokenizer thread terminated"); fflush(stdout));

	return NULL;
}
