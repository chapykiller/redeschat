#ifndef _H_THREAD_
#define _H_THREAD_

#include <pthread.h>

// Struct utilizada pela lista encadeada de threads
typedef struct threadNode{
	pthread_t * thread; // Uma thread

	struct threadNode * next;
	struct threadNode * prev;
} threadNode;

threadNode * threadList; // Lista encadeada de threads

/*
 * Cria uma thread e retorna o ponteiro
*/
pthread_t * createThread();

/*
 * Inicializa as variáveis necessárias
*/
void thread_init();

#endif
