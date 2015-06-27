#include <pthread.h>
#include <stdlib.h>

#include "running.h"
#include "broadcast.h"
#include "connections.h"
#include "threadManagement.h"
#include "hashTable.h"
#include "interface.h"
#include <unistd.h>

int main()
{
    // Struct utilizada para ouvir por novas conexões
    connectionListener *conListener = NULL;

    // Seta running para 1, avisando que o programa está em execução
    startRunning();

    // Inicializações
    thread_init(); // Inicializa a lista de threads
    hash_init(); // Inicializa a tabela hash
    contact_init(); // Inicializa a lista e a fila de contatos

    // Inicializa a socket para ouvir por novos contatos
    connections_listenerCreate(&conListener, 48691);

    // Cria uma thread pra ficar ouvindo por conexões
    pthread_create(createThread(), 0, connections_listen, (void*)conListener);
    // Cria uma thread para avisar para todos os contados que ainda esta online
    pthread_create(createThread(), 0, broadcast_alive, NULL);

    // Inicializa e fica executando a interface
    interface_init();

    /****************************
     * Libera memória
    ****************************/

    // Espera todas as threads terminarem de executar (join)
	threadNode *current;
    threadNode *previous = NULL;
    for(current = threadList; current!=NULL; current = current->next){
        if(previous != NULL)
        {
            // libera memória
            free(previous->thread);
            free(previous);
        }

        pthread_join(*current->thread, NULL);

        previous = current;
    }
    if(previous != NULL)
    {
        // Libera memória
        free(previous->thread);
        free(previous);
    }

    // Libera as memórias das listas e da fila de contatos
    free(conListener);
    contact_exit();
    hash_exit();
	
    pthread_mutex_destroy(&runningMutex); // Destroi o mutex do running

    return 0;
}
