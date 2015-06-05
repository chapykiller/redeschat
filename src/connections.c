#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "running.h"
#include "connections.h"
#include "contact.h"
#include "hashTable.h"
#include "threadManagement.h"
#include "message.h"

/*
 * Aloca e inicializa uma struct usada para ouvir por conexões
*/
int connections_listenerCreate(connectionListener **conListener, int port)
{
    *conListener = (connectionListener*)malloc( sizeof(connectionListener) );
    if(conListener == 0)
    {
        perror("Error allocating connection listener.");
        return -1;
    }

   /* Funcao socket(sin_family,socket_type,protocol_number) retorna um inteiro (socket descriptor), caso erro retorna -1

      O numero do protocolo (protocol_number):
   		0 - IP - Internet Protocol (Default)
   */
   if (((*conListener)->socketvar = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
     perror("Error creating socket");
     return -2;
   }

   int true = 1;

   /* Funcao setsockopt(int socket, int level, int optname, void*optval, size_t optlen)

	Esta funcao seta o valor (optval) de uma opcao (optname) em um certo nivel (level) de camada de protocolo no socket

	int socket = descriptor do socket

	int level = nivel da camada do protocolo (SOL_SOCKET = Constante de nivel para o socket)

	int optname = Opcao desejada para a alteracao

	optval = valor da opcao

	optlen = tamanho do valor

   */
   if (setsockopt((*conListener)->socketvar, SOL_SOCKET, SO_REUSEADDR, (char*)&true, sizeof(int)) == -1)
   {
      perror("Error in Setsockopt");
      return -3;
   }

    // Configura o endereco de destino
    (*conListener)->addr.sin_family = AF_INET;
    (*conListener)->addr.sin_port = htons(port);
    (*conListener)->addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&((*conListener)->addr.sin_zero),8);

    /* Uma vez com o socket criado precisamos informar o endereço ao socket. Para isso utilizamos a funcao bind

	    Funcao bind(int socket, struct sockaddr*addr, size_t length)

   	    int socket = descriptor do socket

   	    struct sockaddr*addr = endereco de destino

   	    size_t length = tamanho do endereco

   	    A funcao bind retorna 0 em caso de sucesso e -1 em caso de erro
    */
    if (bind((*conListener)->socketvar, (struct sockaddr *)&(*conListener)->addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Impossible to bind");
        return -4;
    }

    /* Como estamos criando um listener que ira receber solicitacoes este socket deve ficar aguardando conexoes na
         porta especificada. A funcao listen realiza essa tarefa.

   	    Funcao listen(int socket, unsigned int n) onde;

	    int socket = descriptor do socket

   	    unsigned int n = tamanho da fila de conexoes pendentes

    */
    if (listen((*conListener)->socketvar, 10) == -1)
    {
        perror("Error in Listen");
        return -5;
    }

    return 0;
}

/*
 * Thread responsavel por ficar aguardando novas conexoes
*/
void *connections_listen(void *data)
{
    connectionListener *conListener = (connectionListener*)data;

    // Representa o novo contato conectando
    struct sockaddr_in client_addr;
    socklen_t sin_size;
    // Socket criada para essa conexao
    int connected;

    // Coloca o timeout do accept como 1 segundo
    struct timeval timev;
    timev.tv_sec = 1;
    timev.tv_usec = 0;

    fd_set listener_fd_set;
    FD_ZERO(&listener_fd_set);
    FD_SET(conListener->socketvar, &listener_fd_set);

    // Enquanto o programa nao for fechado
    while(isRunning())
    {
        // Variavel para armazenar o tamanho de endereco do cliente conectado
        sin_size = sizeof(struct sockaddr_in);

        if( (select(conListener->socketvar + 1, &listener_fd_set, NULL, NULL, &timev)) > 0)
        {

            /* Funcao accept(int socket, struct sockaddr*addr, size_t*length_ptr)
	            A funcao accept aceita uma conexao e cria um novo socket para esta conexao

	            int socket = descriptor do socket

	            struct sockaddr*addr = endereco de destino (cliente)

	            size_t*length_ptr = tamanho do endereco de destino
            */
            connected = accept(conListener->socketvar, (struct sockaddr *)&client_addr, &sin_size);

            // Representa o contato que acabou de conectar
            contact *newContact;
            // IP ou endereco desse contato
            char host_name[30];

            // Salva o host_name do contato
            inet_ntop(AF_INET, &(client_addr.sin_addr), host_name, INET_ADDRSTRLEN);

            // Aloca e atribui os valores em newContact
            newContact = contact_create("", host_name);
            if(newContact != NULL)
            {
                // Adiciona o contato na tabela hash usando host_name como chave
                hash_addContact(newContact, newContact->host_name);

                // Atribui o socket
                newContact->socketvar = connected;
            
                // Adiciona para a lista ligada
                queueContact(newContact);
            }
        }

        sleep(1);
    }

    close(conListener->socketvar);

    pthread_exit(0);
}

/*
 * Conecta em um outro computador na rede
*/
int connections_connect(contact *newContact, int port)
{
    // Representa o host
    struct hostent *host;
    // Representa o nó atual
    struct sockaddr_in server_addr;

    if(newContact == 0)
    {
        perror("Contact not allocated");
        return -1;
    }

    // Obtem o host a partir de host_name
    host = gethostbyname(newContact->host_name);
    if(host == NULL)
    {
        perror("Invalid hostname");
        return -2;
    }


    // Coloca o timeout do recv como 1 segundo
    struct timeval timev;
    timev.tv_sec = 1;
    timev.tv_usec = 0;
    
    // Cria a socket
    if ((newContact->socketvar = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error creating socket");
        return -3;
    }

    if (setsockopt(newContact->socketvar, SOL_SOCKET, SO_RCVTIMEO, (char*)&timev, sizeof(struct timeval)) == -1)
    {
        perror("Error in Setsockopt");
        return -4;
    }

    // Atribui as configurações necessarias
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8);

    // Tenta conectar com o contato
    if (connect(newContact->socketvar, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Error during connect");
        return -5;
    }

    pthread_create(createThread(), 0, message_receive, (void*)newContact);

    return 0;
}
