// my headers
#include <fifo-queue.h>
#include <pipeline.h>
// std headers
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

// defines a maximum line size to alloc a buffer
#define MAX_LN_SZ 1000

// Function executed by any thread whose job is to read lines from a FILE *file_pointer
// and put them in a FIFO queue for a tokenizer to process
void *read_line(void *file_pointer)
{
	// just to remind the fact that file_pointer is in fact a pointer to a FILE
	FILE *fpoint = (FILE *)file_pointer;

	// Now the line buffer can be alloc'd
	char *private_lnbuf = calloc(MAX_LN_SZ, sizeof(char));
	if (!private_lnbuf)
	{
		perror("reader: calloc private line buffer");
		return NULL;
	}

	// count the number of lines read from the file
	int lines_read = 0;
	int ret;

	// read the file line by line inside buf
	while (fgets(private_lnbuf, MAX_LN_SZ, fpoint) != NULL)
	{
		lines_read++;

		// Take mutex on the lines queue
		if ((ret = pthread_mutex_lock(&mux_lnbuf)) == -1)
		{
			perror("lock line buffer");
			exit(-1);
		}

		//DBG(printf("Read line: %s\n", private_lnbuf);fflush(stdout));

		// insert the line just read into the queue
		// NOTE: the string is duplicated by enqueue()
		// so it must be freed by who pops the queue
		enqueue(&q_lines_head, &q_lines_tail, private_lnbuf, strnlen(private_lnbuf, MAX_LN_SZ));
		// signal waiting tokenizer thread that a line has been added to the queue
		pthread_cond_signal(&lnbuf_new);

		// release mutex
		if ((ret = pthread_mutex_unlock(&mux_lnbuf)) == -1)
		{
			perror("unlock line buffer");
			exit(-1);
		}
	}
	// no more lines to read, send end of stream token

	// To signal the end of the stream, add one last queue element, consisting of
	// {NULL, NULL}. The element is hand-crafted to avoid overcomplicating enqueue()
	struct Queue *EOstream = malloc(sizeof(struct Queue));
	if (!EOstream)
	{
		perror("cannot create end of stream token");
		exit(-1);
	}
	EOstream->data = NULL;
	EOstream->next = NULL;

	if ((ret = pthread_mutex_lock(&mux_lnbuf)) == -1)
	{
		perror("lock line buffer");
		exit(-1);
	}

	// insert the end of stream token
	if (q_lines_tail)
	{
		q_lines_tail->next = EOstream;
		q_lines_tail = EOstream;
	}
	else
	{
		q_lines_head = q_lines_tail = EOstream;
	}
	// signal as well
	pthread_cond_signal(&lnbuf_new);

	// release mutex
	if ((ret = pthread_mutex_unlock(&mux_lnbuf)) == -1)
	{
		perror("unlock line buffer");
		exit(-1);
	}

	// last thing: free the internal line buffer
	free(private_lnbuf);

	// if all went well then return the number of lines read
	return (void *)lines_read;
}
