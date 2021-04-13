/*
 * fifo-queue.h
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

#ifndef FIFO_QUEUE_INCLUDED
#define FIFO_QUEUE_INCLUDED

#include <stddef.h>

// defines the Queue structure: the data field holds a string
// and the next field a pointer to the next element in the queue
struct Queue {
	char *data;
	struct Queue *next;
};

// enqueue() duplicates the string str of lenght len and inserts it at the tail of the queue
// pointed to by head and tail pointers
void enqueue(struct Queue **head, struct Queue **tail, char *str, size_t len);

// pop() removes the first element from the queue (pointed to by head) and returns it
// If the queue was empty, NULL is returned
// Note that the struct Queue* returned needs to be freed by the caller
struct Queue *pop(struct Queue **head, struct Queue **tail);

#endif
