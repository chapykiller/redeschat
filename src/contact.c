#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "contact.h"

void addMessage(contact * cont, const char * origin, char * message){
	pthread_mutex_lock(&cont->messageMutex);

	char * copy = (char *)malloc(553*sizeof(char));
	strcpy(copy, origin);
	strcat(copy, ": ");
	strcat(copy, message);

	messageNode * newNode = (messageNode *)malloc(sizeof(messageNode));
	newNode->message = copy;

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

	for(current = cont->messages; current!=NULL; current = current->next){
		if(n++ > MAX_MESSAGES){
			current->prev->next = NULL;

			for(; current!=NULL;){
				messageNode * temp = current;
				current = current->next;

				free(temp->message);
				free(temp);
			}
		}
	}

	pthread_mutex_unlock(&cont->messageMutex);

	return;
}

void queueContact(contact * host){
	pthread_mutex_lock(&queueMutex);

	contactNode * newNode = (contactNode *)malloc(sizeof(contactNode));
	newNode->value = host;

	newNode->next = contactQueue;

	pthread_mutex_unlock(&queueMutex);

	return;
}

contact * dequeueContact(){
	pthread_mutex_lock(&queueMutex);

	contact * ret = NULL;

	if(contactQueue != NULL){
		ret = contactQueue->value;
		contactNode * temp = contactQueue;

		contactQueue = contactQueue->next;
		free(temp);
	}

	pthread_mutex_unlock(&queueMutex);

	return ret;
}

contact * contact_create(const char *nickname, const char *host_name)
{
	contact * ret = (contact*)malloc(sizeof(contact));

    if(ret == NULL)
    {
        perror("Error allocating contact");
        return NULL;
    }

    strcpy(ret->host_name, host_name);
    strcpy(ret->nickname, nickname);

    ret->references = 0;

    ret->messages = NULL;
    pthread_mutex_init(&ret->messageMutex, NULL);

    return ret;
}

void contact_init(){
	contactQueue = NULL;
	pthread_mutex_init(&queueMutex, NULL);

	return;
}

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
