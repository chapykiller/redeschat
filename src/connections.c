#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "global.h"
#include "connections.h"
#include "contact.h"
#include "hashTable.h"

int connections_listenerCreate(connectionListener *conListener, int port)
{
    conListener = (connectionListener*)malloc( sizeof(connectionListener) );
    if(conListener = 0)
    {
        perror("Error allocating connection listener.");
        return -1;
    }

    conListener->true = 1;

   /* Funcao socket(sin_family,socket_type,protocol_number) retorna um inteiro (socket descriptor), caso erro retorna -1
   
      O numero do protocolo (protocol_number) pode ser algum dos seguintes:
   		0 - IP - Internet Protocol (Default)
    		1 - ICMP - Internet Control Message Protocol
    		2 - IGMP - Internet Group Multicast Protocol
    		3 - GGP - Gateway-Gateway Protocol
    		6 - TCP - Transmission Control Protocol
    		17 - UDP - User Datagram Protocol	
   */ 
   if ((conListener->socketvar = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
     perror("Error creating socket");
     return -2;
   }
   
   /* Funcao setsockopt(int socket, int level, int optname, void*optval, size_t optlen)
   
	Esta funcao seta o valor (optval) de uma opcao (optname) em um certo nivel (level) de camada de protocolo no socket
	
	int socket = descriptor do socket
   	
	int level = nivel da camada do protocolo (SOL_SOCKET = Constante de nivel para o socket, outros: IPPROTO_IP, IPPROTO_TCP, IPPROTO_UDP)
   	
	int optname = Opcao desejada para a alteracao
   
	optval = valor da opcao
   
	optlen = tamanho do valor

  	Neste exemplo iremos alterar o valor no nivel de socket para a opcao SO_REUSEADDR. Por default um socket criado aceita apenas
   	uma conexao por endereco, ou seja o valor de SO_REUSEADDR é igual FALSE (0). Para alterar esse valor e permitirmos que o
   	mesmo endereco possa receber varias conexoes devemos alterar o valor da opcao SO_REUSEADDR para TRUE (1).

   */
   if (setsockopt(conListener->socketvar, SOL_SOCKET, SO_REUSEADDR, &conListener->true,sizeof(int)) == -1)
   {
      perror("Error in Setsockopt");
      return -3;
   }

    // Configura o endereco de destino
    conListener->addr.sin_family = AF_INET;
    conListener->addr.sin_port = htons(port);
    conListener->addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(conListener->addr.sin_zero),8);
   
    /* Uma vez com o socket criado precisamos informar o endereço ao socket. Para isso utilizamos a funcao bind
   
	    Funcao bind(int socket, struct sockaddr*addr, size_t length)

   	    int socket = descriptor do socket

   	    struct sockaddr*addr = endereco de destino

   	    size_t length = tamanho do endereco

   	    A funcao bind retorna 0 em caso de sucesso e -1 em caso de erro
    */
    if (bind(conListener->socketvar, (struct sockaddr *)&conListener->addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Impossible to bind");
        return -4;
    }

    /* Como estamos criando um servidor que ira receber solicitacoes este socket deve ficar "ouvindo" (aguardando conexoes) na
         porta especificada. A funcao listen realiza essa tarefa.

   	    Funcao listen(int socket, unsigned int n) onde;
   	
	    int socket = descriptor do socket

   	    unsigned int n = tamanho da fila de conexoes pendentes
   
   	    Obs: Quando utilizamos o listen devemos utilizar a funcao accept que veremos mais adiante no codigo

    */
    if (listen(conListener->socketvar, 10) == -1)
    {
        perror("Error in Listen");
        return -5;
    }

    return 0;
}

void *connections_listen(void *data)
{
    connectionListener *conListener = (connectionListener*)data;

    struct sockaddr_in client_addr;
    int sin_size;
    int connected;
    
    while(running)
    {
        // Variavel para armazenar o tamanho de endereco do cliente conectado
        sin_size = sizeof(struct sockaddr_in);

        /* Funcao accept(int socket, struct sockaddr*addr, size_t*length_ptr)
	        A funcao accept aceita uma conexao e cria um novo socket para esta conexao
      	
	        int socket = descriptor do socket
      
	        struct sockaddr*addr = endereco de destino (cliente)
      
	        size_t*length_ptr = tamanho do endereco de destino

      	    Obs: A funcao accept por padrão fica aguardando a chegada de um pedido de conexao. Para que ela nao fique, devemos
      	    configurar o socket no modo sem bloqueio (nonblocking mode set). Neste exemplo ficaremos com o modo padrao (bloqueante)
        */ 
        connected = accept(conListener->socketvar, (struct sockaddr *)&client_addr, &sin_size);

        contact *newContact;
        char nickname[20];

        //TODO receive nickname
        if(contact_create(newContact, nickname, client_addr.sin_addr, connected) == 0)
            hash_addContact(newContact);

        sleep(1);
    }
    
    pthread_exit(0);
}

int connections_connect(contact *newContact, int port)
{
    struct hostent *host;
    struct sockaddr_in server_addr;

    if(newContact = 0)
    {
        perror("Contact not allocated");
        return -1;
    }

    if(hash_retrieveContact(newContact->host_name) != 0)
    {
        perror("Hostname already connected");
        return -2
    }
    
    if ((newContact->socketvar = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error creating socket");
        return -3;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8);
   
    if (connect(newContact->socketvar, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Error during connect");
        return -4;
    }

    return 0;
}
