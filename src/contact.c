#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "contact.h"

void addMessage(contact * cont, char * message){
	char * copy = (char *)malloc(512*sizeof(char));
	strcpy(copy, message);

	messageNode * newNode = (messageNode *)malloc(sizeof(messageNode));
	newNode->message = copy;

	newNode->next = cont->messages;
	newNode->prev = NULL;

	int n = 0;

	if(cont->messages == NULL){
		cont->messages = newNode;
	}else{
		cont->messages->prev = newNode;

		cont->messages = newNode;
	}

	return;
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
