#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "broadcast.h"
#include "message.h"
#include "contact.h"
#include "running.h"
#include "jsonxstr.h"

void broadcast_send(char *message)
{
	contact * current;

	for(current = contactList; current!=NULL; current = current->next){
		if(current->status == STATUS_ALIVE)
            message_send(current, message);
	}
}

void broadcast_dead()
{
    char *deadMessage = makeJSONControl(1);

    broadcast_send(deadMessage);

    free(deadMessage);
}

void *broadcast_alive(void *data)
{
    time_t last_update;

    char *aliveMessage = makeJSONControl(0);

    time(&last_update);

    while(isRunning())
    {
        if( (time(NULL) - last_update) >= 30)
        {
            broadcast_send(aliveMessage);

            time(&last_update);
        }

        sleep(1);
    }

    free(aliveMessage);

    pthread_exit(0);
}
