/*
 * fifo-queue.c
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

// my header, where the queue is defined
#include <fifo-queue.h>
// std lib headers needed
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

void* enqueue(struct Queue **head, struct Queue **tail, void *msg, size_t msg_size) {
	struct Queue *elem = malloc(sizeof(struct Queue));
	if(!elem) {
		return NULL;
	}
	
	assert(msg_size > 0);
	elem->data = malloc(sizeof(void*));
	if(elem->data == NULL) {
	    free(elem);
	    return NULL;
	}
	*(elem->data) = msg;
	
	elem->next = NULL;
	if(*tail) {
		(*tail)->next = elem;
		*tail = elem;
	}
	else {
		*head = elem;
		*tail = elem;
	}
	
	// if all went well returns pointer to the newly inserted element
	return (void*)elem;
}

struct Queue *pop(struct Queue **head, struct Queue **tail)
{
	if(*head) {
		struct Queue *tmp = *head;
		if(*head == *tail) {
			*tail = NULL;
		}
		*head = (*head)->next;
		return tmp; // returns the struct: needs to be freed
	}
	// empty queue
	return NULL;
}
