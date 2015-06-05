#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "threadManagement.h"

pthread_t * createThread(){
	pthread_t * ret = (pthread_t *)malloc(sizeof(pthread_t));

	threadNode * newNode = (threadNode *)malloc(sizeof(threadNode));
	newNode->thread = ret;

	newNode->next = threadList;
	newNode->prev = NULL;

	if(threadList == NULL){
		threadList = newNode;
	}else{

		threadList->prev = newNode;

		threadList = newNode;
	}

	return ret;
}

void thread_init(){
	threadList = NULL;

	return;
}
