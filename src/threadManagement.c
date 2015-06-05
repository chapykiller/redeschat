#include <stdio.h>
#include <pthread.h>

#include "threadManagement.h"

threadNode * threadList;

pthread_t * createThread(){
	pthread_t * ret = (pthread_t *)malloc(sizeof(pthread_t));

	threadNode * newNode = (threadNode *)malloc(sizeof(threadNode));
	newNode->thread = ret;

	if(threadList == NULL){
		newNode->next = NULL;
		newNode->prev = NULL;
	}else{
		newNode->next = threadList;
		newNode->prev = NULL;

		threadList->prev = newNode;

		threadList = newNode;
	}

	return ret;
}

void thread_init(){
	threadList = NULL;

	return;
}