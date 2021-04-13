/*
 * dining-philosopher2.c
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

// pthread header
#include <pthread.h>
// std lib headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <time.h>

// This program solves the dining philosophers by using an ordering of accesses to
// resources (forks), to ensure that no deadlock ensues.
// The program takes two parameters: the number of philosophers (and forks) and
// the amount of times the philosoher (thread) eats before terminating

// Each fork can be held by at most one philosopher at a time,
// so each one must have its own mutex
pthread_mutex_t *mux_forks;

static long n;
static long iter;
static int *forks;

// code executed by the philosopher
void *phi(void *position);
// code to grab in ME forks nearby philosopher at position pos
void grab_forks(long pos);
// code to release ME on grabbed forks
void release_forks(long pos);

// Launch the program and read arguments,
// then create threads and wait for their termination

int main(int argc, char **argv)
{
	
	return 0;
}

