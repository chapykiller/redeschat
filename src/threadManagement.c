#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "threadManagement.h"

/*
 * Cria uma thread e retorna o ponteiro
*/
pthread_t * createThread(){
	pthread_t * ret = (pthread_t *)malloc(sizeof(pthread_t)); // Aloca a memória para uma nova thread

	threadNode * newNode = (threadNode *)malloc(sizeof(threadNode)); // Aloca a memória para um novo nó na lista de threads
	newNode->thread = ret; // Seta a thread desse nó como a thread alocada anteriormente

	// Faz as configurações necessarias para a lista encadeada
    newNode->next = threadList;
	newNode->prev = NULL;

    // Se a lista estava vazia
	if(threadList == NULL){
		threadList = newNode; // A primeira posição vai ser o nó que acabou de ser criado
	}else{
        // Faz as configurações necessárias para a lista encadeada
		threadList->prev = newNode;

		threadList = newNode;
	}

	return ret; // Retorna a thread alocada
}

/*
 * Inicializa as variáveis necessárias
*/
void thread_init(){
	threadList = NULL; // Inicializa a lista de threads como vazia
}
