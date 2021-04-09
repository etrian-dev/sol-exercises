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
#include <string.h>

char *strndup(const char *string, size_t size);

void enqueue(struct Queue **head, struct Queue **tail, char *str, size_t len) {
	struct Queue *elem = malloc(sizeof(struct Queue));
	if(!elem) {
		perror("Lol, malloc just failed");
		exit(-1);
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

char *pop(struct Queue **head, struct Queue **tail) {
	if(*head) {
		struct Queue *tmp = *head;
		char *str = tmp->data;
		if(*head == *tail) {
			*tail = NULL;
		}
		*head = (*head)->next;
		free(tmp);
		return str;
	}
	// empty queue
	return NULL;
}
