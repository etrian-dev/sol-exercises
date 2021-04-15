// my header, where the queue is defined
#include <fifo-queue.h>
// std lib headers needed
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

// c99 doesn't know this function
char *strndup(const char *string, size_t size);

void enqueue(struct Queue **head, struct Queue **tail, char *str, size_t len) {
	struct Queue *elem = malloc(sizeof(struct Queue));
	if(!elem) {
		perror("[enqueue] malloc failed");
		exit(1); // brutally exit process
	}
	elem->data = strndup(str, len);
	elem->next = NULL;
	if(*tail) {
		(*tail)->next = elem;
		*tail = elem;
	}
	else {
		*head = elem;
		*tail = elem;
	}
}

struct Queue *pop(struct Queue **head, struct Queue **tail)
{
	if(*head) {
		struct Queue *tmp = *head;
		if(*head == *tail) {
			*tail = NULL;
		}
		*head = (*head)->next;
		return tmp; // returns the struct: needs to be freed by the caller
	}
	// empty queue
	return NULL;
}
