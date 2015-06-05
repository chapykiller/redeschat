#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "contact.h"
#include "message.h"
#include "global.h"

#define MESSAGE_MAXSIZE 519

int message_send(contact *receiver, char *send_data)
{
    if(receiver == 0)
    {
        perror("Receiver doesn't exist");
        return -1;
    }

    send(receiver->socketvar, send_data, strlen(send_data), 0);

    return 0;
}

void *message_receive(void *data)
{
    contact *sender = (contact*)data;

    int bytes_recv;
    char recv_data[MESSAGE_MAXSIZE];

    while(running)
    {
        if(sender->status == STATUS_DEAD)
        {
            close(sender->socketvar);
            pthread_exit(0);
        }

        bytes_recv = recv(sender->socketvar, recv_data, MESSAGE_MAXSIZE, 0);

        // Se ocorreu timeout, checa se o cliente pode estar disconectado
        if(bytes_recv == 0)
        {
            /* Se passou mais de 40 segundos desde a ultima mensagem
                do nó, seja ela uma mensagem normal ou de controle
            */
            if( (time(NULL) - sender->lastUpdate) > 40)
            {
                sender->status = STATUS_DEAD; // O nó disconectou
                close(sender->socketvar);
                pthread_exit(0);
            }
        }
        else
        {
            recv_data[bytes_recv] = '\0';
            // TODO funcao magica e tals
        }

        sleep(1);
    }
    
    pthread_exit(0);
}
