#include <pthread.h>

#include "running.h"

/*
 * Inicia running como 1
*/
void startRunning()
{
    // Inicializa o mutex
    pthread_mutex_init(&runningMutex, NULL);

    pthread_mutex_lock(&runningMutex);

    running = 1; // Atribui running com o valor 1 -> está executando

    pthread_mutex_unlock(&runningMutex);
}

/*
 * Retorna o valor de running
*/
int isRunning()
{
    int aux; // Variável auxiliar para retornar o valor

    pthread_mutex_lock(&runningMutex);

    aux = running; // Obtém o valor de running

    pthread_mutex_unlock(&runningMutex);

    return aux; // Retorna se esta executando ou não
}

/*
 * Para a execução do programa
 * fazendo running = 0
*/
void stopRunning(){
	pthread_mutex_lock(&runningMutex);

	running = 0; // Atribui running com o valor 0 -> não está executando

	pthread_mutex_unlock(&runningMutex);
}
