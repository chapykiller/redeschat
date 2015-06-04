#ifndef _CONNECTIONS_H_
#define _CONNECTIONS_H_

#include <netinet/in.h>

#include "contact.h"

typedef struct sConnectionListener
{
    int socketvar;
    int true;
    struct sockaddr_in addr;
} connectionListener;

int connections_listenerCreate(connectionListener *conListener, int port);

void *connections_listen(void *data);

int connections_connect(contact *newContact, int port);

#endif //_CONNECTIONS_H_
