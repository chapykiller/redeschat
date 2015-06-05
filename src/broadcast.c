#include "broadcast.h"
#include "message.h"
#include "contact.h"
#include "global.h"
#include "jsonxstr.h"

void broadcast_send(char *message)
{
	contact * current;

	for(current = contactList; current!=NULL; current = current->next){
		if(current->status == STATUS_ALIVE)
            message_send(current, message);
	}
}

void *broadcast_alive(void *data)
{
    char *aliveMessage = makeJSONMessage(/*TODO*/);

    while(running)
    {
        broadcast_send(aliveMessage);
        sleep(30);
    }

    pthread_exit(0);
}
