#include <pthread.h>
#include <stdlib.h>

#include "global.h"
#include "broadcast.h"
#include "connections.h"
#include "threadManagement.h"

int main()
{
    connectionListener *conListener;

    running = 1;

    pthread_mutex_init(&hashMutex, NULL);

    connections_listenerCreate(conListener, 2134);

    pthread_create(createThread(), 0, connections_listen, (void*)conListener);
    pthread_create(createThread(), 0, broadcast_alive, NULL);

    free(conListener);

    return 0;
}
