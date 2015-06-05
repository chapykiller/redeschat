#include <pthread.h>

#include "running.h"

void startRunning()
{
    pthread_mutex_init(&runningMutex, NULL);

    pthread_mutex_lock(&runningMutex);
    
    running = 1;
    
    pthread_mutex_unlock(&runningMutex);
}

int isRunning()
{
    int aux;

    pthread_mutex_lock(&runningMutex);

    aux = running;

    pthread_mutex_unlock(&runningMutex);

    return aux;
}

void stopRunning(){
	pthread_mutex_lock(&runningMutex);

	running = 0;

	pthread_mutex_unlock(&runningMutex);
}