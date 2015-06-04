#ifndef _CONNECTIONLISTENER_H_
#define _CONNECTIONLISTENER_H_

#include <netinet/in.h>

typedef struct sConnectionListener
{
    int sock;
    int true;
    struct sockaddr_in addr;
} connectionListener;

int connectionListenerCreate(connectionListener *conListener, int port);

void *connectionListenerListen(void *data);

#endif //_CONNECTIONLISTENER_H_
