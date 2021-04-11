// my headers
#include <icl_hash.h> // library for the dictionary
#include <fifo-queue.h>
#include <pipeline.h>
// std headers
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

void free_k(void *key) {
	free(key);
}

void *print_line(void *unused)
{
	// stream termination flag, set by the special token {NULL, NULL}
	int end_of_stream = 0;

	// create an hashtable to act as a dictionary first
	icl_hash_t *dictionary;
	// 10000 buckets, hashed with pwj hash function and the string comparison function
	dictionary = icl_hash_create(10000, hash_pjw, string_compare);
	if(!dictionary) {
		printf("Printer: Failed to create dictionary\n");
		return NULL;
	}

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
			// insert token in the hashtable if it's not already in there
			if(icl_hash_find(dictionary, (void*)elem->data) == NULL) {
				void *ret = icl_hash_insert(dictionary, (void*)elem->data, (void*)elem->data);
				if(ret == NULL) {
					printf("Failed to insert token \'%s\' in the dict\n", elem->data);
					return NULL;
				}
			}
			else {
				free(elem->data);
			}
			free(elem);
		}
	}

	// print all the items in the dictionary
	icl_entry_t *e;
	for(int i = 0; i < dictionary->nbuckets; i++) {
		e = dictionary->buckets[i];
		while(e != NULL) {
			// print the data part
			printf("%s\n", (char*)e->data);

			// get the next token
			e = e->next;
		}
	}

	// destroy the hashtable (and free its memory)
	if(icl_hash_destroy(dictionary, free_k, NULL) == -1) {
		printf("Printer: failed to destroy hashtable");
		return NULL;
	}
	
	//DBG(printf("Printer thread terminated"); fflush(stdout));

	return NULL;
}
