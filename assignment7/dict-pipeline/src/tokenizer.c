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
	// stream termination flag, set by the special token {NULL, NULL}
	int end_of_stream = 0;
	while (!end_of_stream)
	{
		int ret;
		// Take mutex on the lines queue
		if ((ret = pthread_mutex_lock(&mux_lnbuf)) == -1)
		{
			perror("lock line buffer");
			exit(-1);
		}

		// wait until there are no tokens in the queue
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
			// special termination token: exit tokenizer and insert the termination token
			// in the token stream as well
			end_of_stream = 1;

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
		// process the token obtained
		else
		{
			//DBG(printf("Popped line: %s\n", elem->data);fflush(stdout));

			// elem->data contains a line. The string is tokenized
			// and its tokens will be added to the tokens queue (in ME)
			char *strtok_state = NULL;
			char *token = strtok_r(elem->data, " ", &strtok_state);
			while (token != NULL)
			{
				//DBG(printf("Token: %s\n", token);fflush(stdout));

				// take mutex on the tokens queue
				if ((ret = pthread_mutex_lock(&mux_tokbuf)) == -1)
				{
					perror("lock line buffer");
					exit(-1);
				}

				// enqueue the token on the tokens queue
				enqueue(&q_tokens_head, &q_tokens_tail, token, strlen(token));
				// signal the printer thread that a new token has been added
				pthread_cond_signal(&tokbuf_new);

				// release mutex
				if ((ret = pthread_mutex_unlock(&mux_tokbuf)) == -1)
				{
					perror("unlock line buffer");
					exit(-1);
				}

				// get the next token
				token = strtok_r(NULL, " ", &strtok_state);
			}
			// free the processed line
			free(elem->data);
			free(elem);
		}
	}

	//DBG(printf("Tokenizer thread terminated"); fflush(stdout));

	return NULL;
}
