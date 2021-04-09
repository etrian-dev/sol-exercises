#ifndef PIPELINE_H_INCLUDED
#define PIPELINE_H_INCLUDED

// my library implementing a FIFO queue
#include <fifo-queue.h>
// std headers
#include <pthread.h>
#include <stddef.h>

// useful macro to strip debugging prints from the executable
#ifdef DEBUG
#define DBG(X) X
#else
#define DBG(X)
#endif

// defining them because c99 complains
char *strdup(const char *s);
char *strndup(const char *s, size_t sz);
size_t strnlen(const char *s, size_t maxlen);

// functions executed by each thread
void *read_line(void *file_desc);
void *tokenize_line(void *unused);
void *print_line(void *unused);

// global queues and syncronization variables, defined in pipeline.c
extern struct Queue *q_lines_head;
extern struct Queue *q_lines_tail;
extern struct Queue *q_tokens_head;
extern struct Queue *q_tokens_tail;
extern pthread_mutex_t mux_lnbuf;
extern pthread_mutex_t mux_tokbuf;
extern pthread_cond_t lnbuf_new;
extern pthread_cond_t tokbuf_new;

#endif