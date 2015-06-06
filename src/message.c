#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "contact.h"
#include "message.h"
#include "running.h"
#include "jsonxstr.h"

/*
 * Envia uma mensagem já codificada utilizando json
 * para um contato especifico
*/
int message_send(contact *receiver, char *send_data)
{
    // Se foi passado um contato inválido
    if(receiver == 0)
    {
        perror("Receiver doesn't exist");
        return -1;
    }

    // Envia a mensagem
    send(receiver->socketvar, send_data, strlen(send_data), 0);

    return 0;
}

/*
 * Thread responsavel por receber mensagens de outros
 * contatos
 * (void*)data é do tipo contact*
*/
void *message_receive(void *data)
{
    contact *sender = (contact*)data;

    // Quantidade de bytes recebidos
    int bytes_recv;
    // Dados recebidos
    char recv_data[MESSAGE_MAXSIZE] = "";
    // Dados incompletos do ultimo recv que estao esperando o restante da mensagem
    // Tambem armazena a concatenacao desses dados com os do recv atual
    char cat_message[2 * MESSAGE_MAXSIZE] = "";

    // Enquanto o programa nao for fechado
    while(isRunning())
    {
        if(sender != NULL)
        {
            // Fecha o socket e finaliza o thread se o contato estiver desconectado
            if(sender->status == STATUS_DEAD)
            {
                close(sender->socketvar);
                pthread_exit(0);
            }

            // Recebe os dados
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
                    close(sender->socketvar); // Fecha o socket
                    pthread_exit(0);
                }
            }
            else
            {
                // Armazena quantos caracteres contem a mensagem json valida
                int length;
                // Armazena uma mensagem json valida
                char *json_message;
                // Usado como auxiliar para validar as mensagens json
                char *aux_recvData;

                // Tranforma os dados recebidos em uma string valida
                recv_data[bytes_recv] = '\0';
                // Concatena a mensagem incompleta do recv anterior com os dados do recv atual
                strcat(cat_message, recv_data);

                // Comeca apontado para a primeira posicao de cat_message
                aux_recvData = cat_message;

                do
                {
                    // Obtem uma mensagem json valida
                    json_message = validateJSON(aux_recvData, &length);

                    // Se a mensagem não for valida ou chegou ao fim da string
                    if(json_message == NULL)
                    {
                        char aux[2 * MESSAGE_MAXSIZE] = "";

                        // Copia para cat_message
                        strcpy(aux, aux_recvData);
                        strcpy(cat_message, aux);
                    }
                    else
                    {
                        // Desloca para a proxima mensagem
                        aux_recvData += length + 1;
                        // Se ultrapassou o limite do buffer faz apontar para o final dele
                        if( (aux_recvData - (cat_message + strlen(cat_message))) >= 0)
                            aux_recvData = cat_message + strlen(cat_message);

                        // Transforma a mensagem json em uma mensagem normal
                        decodeJSON(json_message, sender);

                        free(json_message);
                    }
                }
                while(json_message != NULL);

                sender->lastUpdate = time(NULL);
            }
        }

        sleep(1);
    }

    pthread_exit(0);
}
