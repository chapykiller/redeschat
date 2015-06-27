#ifndef _RUNNING_H_
#define _RUNNING_H_

#include <pthread.h>

int running; // Indica se o programa ainda vai continuar executando

pthread_mutex_t runningMutex; // Mutex para a variável running

/*
 * Inicia running como 1
*/
void startRunning();

/*
 * Retorna o valor de running
*/
int isRunning();

/*
 * Para a execução do programa
 * fazendo running = 0
*/
void stopRunning();

#endif //_RUNNING_H_
