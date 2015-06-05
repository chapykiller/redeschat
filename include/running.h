#ifndef _RUNNING_H_
#define _RUNNING_H_

#include <pthread.h>

int running;
pthread_mutex_t runningMutex;

void startRunning();

int isRunning();

#endif //_RUNNING_H_
