#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "running.h"
#include "hashTable.h"
#include "connections.h"
#include "jsonxstr.h"
#include "message.h"
#include "contact.h"

char messageTarget[31];

int checkNickname(char * nick){
	int i;

	for(i=0; nick[i]!='\0'; i++){
		if(!isValidNick(nick[i]))
			return 0;
	}

	return 1;
}

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
	char * nickname = (char *)malloc(51*sizeof(char));

	int r1 = sscanf(input, "%30s", hostname);
	int r2 = sscanf(input, "%50s", nickname);

	if(r1 == EOF || r2 == EOF){
		printf("Syntax is wrong. Please consult /help if you need to.%s", seq);
	}else{
		if(!checkNickname(nickname)){
			printf("Nickname cotains illegal characters. Please use only letters and numbers.%s", seq);
		}else{
			int i, length = 0;

			for(i=0; nickname[i]!='\0'; i++){
				length++;
			}

			if(length > 20){
				printf("Nickname is way too long.%s", seq);
			}else if(length == 0){
				printf("Please type a nickname.%s", seq);
			}else{
				contact * contact_by_host = hash_retrieveContact(hostname);
				contact * contact_by_nick = hash_retrieveContact(nickname);

				if(contact_by_host != NULL && contact_by_host->status == STATUS_ALIVE){
					printf("A contact with this hostname already exists.%s", seq);
				}else{
					if(contact_by_nick != NULL && contact_by_nick->status == STATUS_ALIVE){
						printf("A contact with this nickname already exists.%s", seq);
					}else{
						if(contact_by_nick != NULL)
							hash_removeContact(nickname);
						if(contact_by_host != NULL)
							hash_removeContact(hostname);

						contact * newContact = contact_create(hostname, nickname);

						if(connections_connect(newContact, 48691) < 0){
							printf("Failed to estabilish connection.");
						}
					}
				}
			}

		}
	}

	free(hostname);
	free(nickname);

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

void processInboundConnections(contact * var){
	if(var == NULL)
		return;

	printf("\n");
	printf("Inbound connections detected. Please give them a nickname.");

	for(; var!=NULL; var = dequeueContact()){
		int valid;

		char * nickname = (char *)malloc(64*sizeof(char));

		do{
			printf("\n\n Hostname: %s > ", var->host_name);
			fgets(nickname, 64, stdin);

			int nl = 0;
			valid = 0;

			int i;

			for(i=0; nickname[i]!='\0'; i++){
				if(nickname[i]=='\n')
					nl = 1;
			}

			if(!nl){
				do{
					fgets(nickname, 64, stdin);

					for(i=0; nickname[i]!='\0'; i++){
						if(nickname[i]=='\n')
							nl = 1;
					}
				}while(!nl);

				printf("Contact name is way too long.");
			}else{
				int length = 0;

				for(i=0; nickname[i]!='\n'; i++)
					length++;
				nickname[i] = '\0';

				if(length == 0){
					printf("Please type a nickname.");
				}else{
					if(length > 20){
						printf("Contact name is way too long.\n");
					}else{
						if(!checkNickname(nickname)){
							printf("Nickname cotains illegal characters. Please use only letters and numbers.");
						}else{
							contact * contact_by_nick = hash_retrieveContact(nickname);

							if(contact_by_nick != NULL && contact_by_nick->status == STATUS_ALIVE){
								printf("A contact with this name already exists.");
							}else{
								valid = 1;
								strcpy(var->nickname, nickname);
							}
						}
					}
				}
			}
		}while(!valid);
	}

	printf("\n");

	return;
}

int interface_init(){
	int i;

	char seq[] = "\n\n\t> ";

	printf("/tYet Another P2P Chat (YAPC)\n");
	printf("\nType /help if you need help.%s", seq);

	messageTarget[0] = '\0';

	while(isRunning()){
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
			for(i=0; input[i]!='\0'; i++)
				if(input[i]=='\n'){
					input[i] = '\0';
					break;
				}

			processInboundConnections( dequeueContact() );

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
