#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "hashTable.h"
#include "connections.h"
#include "jsonxstr.h"
#include "message.h"
#include "contact.h"

char messageTarget[31];

void displayContacts(char seq[]){
	contact * current;

	if(contactList != NULL){
		int count = 0;

		for(current = contactList; current!=NULL; current = current->next){
			if(current->status == STATUS_ALIVE)
				printf(" - %s > %s\n", current->host_name, current->nickname);
		}

		printf("\nListed a total of %d contacts.%s", count, seq);
	}else{
		printf("Contact list is empty. :(%s", seq);
	}

	return;
}

void addContact(char * input, char seq[]){
	char * hostname = (char *)malloc(31*sizeof(char));
	char * nickname = (char *)malloc(21*sizeof(char));

	int r1 = sscanf(input, "%30s", hostname);
	int r2 = sscanf(input, "%20s", nickname);

	if(r1 == EOF || r2 == EOF){
		printf("Syntax is wrong. Please consult /help if you need to.%s", seq);
	}else{
		contact * newContact = contact_create(hostname, nickname);

		connections_connect(newContact, 4869);
	}

	return;
}

void doMsg(char * input, char seq[]){
	char * arg1 = (char *)malloc(31*sizeof(char));
	char * arg2 = (char *)malloc(512*sizeof(char));

	int r1 = sscanf(input, "%30s", arg1);
	int r2 = sscanf(input, "%[^\n]", arg2);

	if(r1 == EOF){
		printf("Syntax is wrong. Please consult /help if you need to.%s", seq);
	}else{
		contact * target = hash_retrieveContact(arg1);

		if(target == NULL){
			printf("Contact is missing. Review your parameters.%s", seq);
		}else{
			if(target->status == STATUS_DEAD)
				printf("Contact is down. Oh well.%s", seq);
			else{
				if(r2 == EOF){
					strcpy(messageTarget, arg1);
				}else{
					addMessage(target, "You", arg2);
					
					char * json_msg = makeJSONMessage(arg2);
					message_send(target, json_msg);
				}
			}
		}
	}

	free(arg1);
	free(arg2);
}

void displayMessages(char * input, char seq[]){
	char * arg1 = (char *)malloc(31*sizeof(char));
	int r1 = sscanf(input, "%30s", arg1);

	if(r1 == EOF){
		printf("You need to inform the contact you exchanged messages with to display the messages exchanged.%s", seq);
	}else{
		contact * target = hash_retrieveContact(arg1);

		if(target == NULL){
			printf("Contact is missing. Review your parameters.%s", seq);
		}else{
			messageNode * current;

			for(current = target->messages; current!=NULL && current->next!=NULL; current = current->next);
			for(; current!=NULL; current = current->prev){
				printf(" || %s\n", current->message);
			}

			if(target->status == STATUS_DEAD)
				printf("\nThis contact is no longer available.%s", seq);
			else{
				printf("\nMessages displayed.%s", seq);
			}
		}
	}

	free(arg1);
}

int interface_init(){
	int i;

	char seq[] = "\n\n\t> ";

	printf("/tYet Another P2P Chat (YAPC)\n");
	printf("\nType /help if you need help.%s", seq);

	messageTarget[0] = '\0';

	while(running){
		char * input = (char *)malloc(512*sizeof(char));
		fgets(input, 512, stdin);

		int nl = 0;
		for(i=0; input[i]!='\0'; i++)
			if(input[i]=='\n')
				nl = 1;

		if(!nl){
			do{
				nl = 0;
				fgets(input, 512, stdin);

				for(i=0; input[i]!='\0'; i++)
					if(input[i]=='\n')
						nl = 1;
			}while(!nl);

			printf("Your command is way too long. Restrict it to 512 characters.%s", seq);
		}
		else{
			if(input[0] == '/'){
				char * command = (char *)malloc(24*sizeof(char));
				sscanf(input, "%23s", command);

				if(cmp(command, "/help")){
					printf("\n");
					printf("Yet Another P2P Chat provides the following commands:\n\n");
					printf(" - /help\t\t\tIt must be pretty obvious what this command does.\n");
					printf(" - /add <hostname> <nickname>\tAdds contact with IP hostname under the name of nickname.\n");
					printf(" - /list\t\t\tLists all your contacts.\n");
					printf(" - /quit\t\t\tQuits the application. :(\n");
					printf(" - /display <contact>\t\tDisplays the last %d messages with contact.\n", MAX_MESSAGES);
					printf(" - /msg <receiver>\t\tChanges the chat focus to receiver. They will start receiving every message you send.\n");
					printf(" - /msg <receiver> <message>\tSends message to receiver without changing chat focus.%s", seq);
				}else if(cmp(command, "/list")){
					printf("\n");

					displayContacts(seq);
				}else if(cmp(command, "/quit")){
					running = 0;
				}else if(cmp(command, "/add")){
					printf("\n");

					addContact(input, seq);
				}else if(cmp(command, "/msg")){
					printf("\n");

					doMsg(input, seq);
				}else if(cmp(command, "/display")){
					printf("\n");

					displayMessages(input, seq);
				}else if(cmp(command, "/msg")){

				}else{
					printf("\nInvalid command. Type /help if you're lost.%s", seq);
				}

				free(command);
			}else{
				printf("\n");
				contact * target = hash_retrieveContact(messageTarget);

				if(target == NULL){
					printf("No contact set to message to. Need any /help?%s", seq);
				}else{
					if(target->status == STATUS_DEAD)
						printf("Your target contact disconnected. Message not sent.%s", seq);
					else{
						/*char * msg = (char *)malloc(512*sizeof(char));

						sscanf(input, "%s", msg);
						sscanf(input, "%[^\n]", msg);*/

					    // Salva a mensagem no historico
					   	addMessage(target, "You", input);

						char * json_msg = makeJSONMessage(input);
						message_send(target, json_msg);

						//free(msg);
					}
				}
			}
		}

		free(input);
	}

	return 0;
}
