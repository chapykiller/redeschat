#ifndef _CONNECTIONS_H_
#define _CONNECTIONS_H_

#include <netinet/in.h>

#include "contact.h"

// Struct usada para verificar novos pedidos de conexão
typedef struct sConnectionListener
{
    int socketvar;
    struct sockaddr_in addr;
} connectionListener;

/*
 * Aloca e inicializa uma struct usada para ouvir por conexões
*/
int connections_listenerCreate(connectionListener **conListener, int port);

/*
 * Thread responsavel por ficar aguardando novas conexoes
*/
void *connections_listen(void *data);

/*
 * Conecta em um outro computador na rede
*/
int connections_connect(contact *newContact, int port);

#endif //_CONNECTIONS_H_
