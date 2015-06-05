#ifndef _H_THREAD_
#define _H_THREAD_

#include <pthread.h>

struct threadNode;

typedef struct threadNode{
	pthread_t * thread;

	struct threadNode * next;
	struct threadNode * prev;
} threadNode;

pthread_t * createThread();
void thread_init();

#endif