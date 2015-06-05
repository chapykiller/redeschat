#include <pthread.h>
#include <stdlib.h>

#include "global.h"
#include "broadcast.h"
#include "connections.h"
#include "threadManagement.h"
#include "hashTable.h"

int main()
{
    connectionListener *conListener = NULL;

    running = 1;

    thread_init();
    hash_init();

    connections_listenerCreate(&conListener, 2134);

    pthread_create(createThread(), 0, connections_listen, (void*)conListener);
    pthread_create(createThread(), 0, broadcast_alive, NULL);

    running = 0;

	threadNode *current;
    threadNode *previous = NULL;
    for(current = threadList; current!=NULL; current = current->next){
        if(previous != NULL)
            free(previous);

        pthread_join(*current->thread, NULL);

        previous = current;
    }
    if(previous != NULL)
        free(previous);

    free(conListener);

    return 0;
}
