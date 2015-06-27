#ifndef _CONTACT_H_
#define _CONTACT_H_

#define STATUS_ALIVE 0
#define STATUS_DEAD 1

#include <pthread.h>

#define MAX_MESSAGES 20

// Struct utilizada para a lista encadeada de mensagens
typedef struct messageNode{
	char * message; // Mensagem em si

	struct messageNode * next;
	struct messageNode * prev;
}messageNode;

// Struct que representa um contato
typedef struct sContact
{
    char host_name[31]; // Endereço IP ou endereco que ser traduzido por DNS
    char nickname[21]; // Apelido do contato

    int status; // Informa se esta conectado ou offline
    int lastUpdate; // Armazena o timestamp desde a ultima mensagem (usado para descobrir se desconectou)

    struct sContact * next;
    struct sContact * prev;

    int references; // Quantidade de referencias na tabela Hash

    messageNode * messages; // Lista encadeada de mensagens
    pthread_mutex_t messageMutex; // Mutex para acesso às mensagens

    int socketvar; // Socket
} contact;

// Struct utilizada para a fila de contatos esperando por nickname
typedef struct contactNode{
    contact * value; // Contato

    struct contactNode * next;
} contactNode;

contact * contactList; // Primeira posição da lista de contatos
contactNode * contactQueue; // Primeira posição da fila de contatos esperando por nickname
pthread_mutex_t queueMutex; // Mutex para acessar a fila

/*
 * Aloca a memória para criar um novo contato
*/
contact * contact_create(const char *nickname, const char *host_name);

/*
 * Adiciona uma mensagem na lista encadeada de um contato
*/
void addMessage(contact * cont, const char * origin, char * message);

/*
 * Coloca um novo contato esperando por nickname na fila
*/
void queueContact(contact * host);

/*
 * Remove o contato da fila
*/
contact * dequeueContact();

/*
 * Inicia as variáveis necessárias
*/
void contact_init();

/*
 * Libera a memória das variáveis que foram alocadas
*/
void contact_exit();

#endif
