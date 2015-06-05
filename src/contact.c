#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "contact.h"

void addMessage(contact * cont, const char * origin, char * message){
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

	return;
}

void queueContact(contact * host){
	contactNode * newNode = (contactNode *)malloc(sizeof(contactNode));
	newNode->value = host;

	newNode->next = contactQueue;
}

contact * dequeueContact(){
	if(contactQueue == NULL){
		return NULL;
	}else{
		contact * ret = contactQueue->value;
		contactNode * temp = contactQueue;

		contactQueue = contactQueue->next;
		free(temp);

		return ret;
	}
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

    return ret;
}

void contact_init(){
	contactQueue = NULL;

	return;
}
