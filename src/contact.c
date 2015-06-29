#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "contact.h"

/*
	Adiciona uma mensagem message ao histórico de mensagens do contato
	cont, que veio de origin.
*/
void addMessage(contact * cont, const char * origin, char * message){
	// Dá lock no mutex da lista encadeada de mensagens.
	pthread_mutex_lock(&cont->messageMutex);

	// Lê o tempo.
	time_t rawtime;
    time(&rawtime);

    struct tm * timestr = localtime(&rawtime);

    // Inicializa a string da mensagem.
	char * copy = (char *)malloc(561*sizeof(char));
	sprintf(copy, "[%02d:%02d] ", timestr->tm_hour, timestr->tm_min);	// Concatena a timestamp.
	strcat(copy, origin);					// Concatena a origem da mensagem
	strcat(copy, ": ");						// Concate um delimitador
	strcat(copy, message);					// Concatena a mensagem.

	// Cria um novo nó para inserir na lista ligada de mensagens.
	messageNode * newNode = (messageNode *)malloc(sizeof(messageNode));
	newNode->message = copy;

	// Insere o novo nó na lista ligada
	newNode->next = cont->messages;
	newNode->prev = NULL;

	if(cont->messages == NULL){
		cont->messages = newNode;
	}else{
		cont->messages->prev = newNode;

		cont->messages = newNode;
	}

	messageNode * current;
	int n = 0;

	// Conta as mensagens e, se houver mais que o permitido, 20, então remove as mensagens
	// excedentes.
	for(current = cont->messages; current!=NULL; current = current->next){
		if(n++ > MAX_MESSAGES){
			current->prev->next = NULL;

			for(; current!=NULL;){
				messageNode * temp = current;
				current = current->next;

				free(temp->message);
				free(temp);
			}

			break;
		}
	}

	// Libera o mutex.
	pthread_mutex_unlock(&cont->messageMutex);

	return;
}

/*
	Serve o propósito de permitir uma manipulação mais fácil
	da queue de inbound connections. No caso, este função adiciona
	um novo contato na queue.
*/
void queueContact(contact * host){
	// Dá lock no mutex da queue.
	pthread_mutex_lock(&queueMutex);

	// Aloca um novo nó
	contactNode * newNode = (contactNode *)malloc(sizeof(contactNode));
	newNode->value = host;

	// Insere na lista.
	newNode->next = contactQueue;

	contactQueue = newNode;

	// Dá unlock no mutex
	pthread_mutex_unlock(&queueMutex);

	return;
}

/*
	Serve o mesmo propósito da função acima, mas faz a operação dequeue
	em um contato, ao invés disso.
*/
contact * dequeueContact(){
	// Dá lock no mutex da queue.
	pthread_mutex_lock(&queueMutex);

	contact * ret = NULL;

	if(contactQueue != NULL){
		// Salva em ret um contato pendente.
		ret = contactQueue->value;
		// Salva temporariamente o primeiro nó para poder removê-lo posteriormente.
		contactNode * temp = contactQueue;

		// Coloca o novo começo da fila.
		contactQueue = contactQueue->next;
		// Libera memória
		free(temp);
	}

	// Dá unlock no mutex.
	pthread_mutex_unlock(&queueMutex);

	return ret;
}

/*
	Inicializa um novo contato, mas não o adiciona na tabela hash ainda.
*/
contact * contact_create(const char *nickname, const char *host_name)
{
	// Aloca memória.
	contact * ret = (contact*)malloc(sizeof(contact));

    if(ret == NULL)
    {
        perror("Error allocating contact");
        return NULL;
    }

    // Preenche os campos de hostname e nickname
    strcpy(ret->host_name, host_name);
    strcpy(ret->nickname, nickname);

    // Inicializa número de referências.
    ret->references = 0;

    // Inicializa ponteiro de histórico de mensagens.
    ret->messages = NULL;

    pthread_mutex_init(&ret->messageMutex, NULL);

    return ret;
}

/*
	Inicializa a contactQueue e seu mutex.
*/
void contact_init(){
	contactQueue = NULL;
	pthread_mutex_init(&queueMutex, NULL);

	return;
}

/*
	Libera a memória de todos os nós de contactQueue ao sair da aplicação.
*/
void contact_exit(){
    contact *temp = dequeueContact();
    while(temp != NULL)
    {
        pthread_mutex_destroy(&temp->messageMutex);
        free(temp);

        temp = dequeueContact();
    }
    pthread_mutex_destroy(&queueMutex);
}
