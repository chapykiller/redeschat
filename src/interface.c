#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include <pthread.h>

#include "running.h"
#include "hashTable.h"
#include "connections.h"
#include "jsonxstr.h"
#include "message.h"
#include "contact.h"
#include "broadcast.h"
#include "threadManagement.h"

char messageTarget[31];

int checkNickname(char * nick){
	int i;

	for(i=0; nick[i]!='\0'; i++){
		if(!isValidNick(nick[i]))
			return 0;
	}

	return 1;
}

char ** decompose(char * str, int * args){
	int i, j, narg = 0;
	int reading = 0;
	int current = 0;

	for(i=0; str[i]!='\n' && str[i]!= '\0'; i++){
		if(str[i] == ' '){
			if(reading == 1){
				reading = 0;
			}
		}else{
			if(reading == 0){
				narg++;
				reading = 1;
			}
		}
	}

	char ** ret = (char **)malloc(narg*sizeof(char *));

	reading = 0;

	for(i=0; str[i]!='\n' && str[i]!= '\0'; i++){
		if(str[i] == ' '){
			if(reading == 1){
				reading = 0;
			}
		}else{
			int size, k;

			if(reading == 0){
				narg++;

				for(j=i, size=0; str[j]!='\n' && str[j]!= '\0' && str[j]!=' '; j++)
					size++;

				ret[current] = (char *)malloc((size+1)*sizeof(char));
				for(j=i, k=0; str[j]!='\n' && str[j]!= '\0' && str[j]!=' '; j++)
					ret[current][k++] = str[j];
				ret[current][k] = '\0';

				current++;
				reading = 1;
			}

		}
	}

	*args = current;

	return ret;
}

void freeMatrix(char ** m, int n){
	int i;

	for(i=0; i<n; i++)
		free(m[i]);
	free(m);


	return;
}

void displayContacts(char seq[]){
	contact * current;

    pthread_mutex_lock(&hashMutex);

	if(contactList != NULL){
		int count = 0;

		for(current = contactList; current!=NULL; current = current->next){
			if(current->status == STATUS_ALIVE)
            {
				printf(" - %s > %s\n", current->host_name, current->nickname);
				count++;
            }
		}

		printf("\nListed a total of %d contacts.%s", count, seq);
	}else{
		printf("Contact list is empty. :(%s", seq);
	}

    pthread_mutex_unlock(&hashMutex);

	return;
}

void addContact(char * input, char seq[]){
	int narg;
	char ** args = decompose(input, &narg);
	char * hostname = NULL;
	char * nickname = NULL;

	if(narg != 3){
		printf("Syntax is wrong. Please consult /help if you need to.%s", seq);
	}else{
	    hostname = args[1];
        nickname = args[2];

		if(!checkNickname(nickname)){
			printf("Nickname cotains illegal characters. Please use only letters and numbers.%s", seq);
		}else{
			int length = strlen(nickname);

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

						contact * newContact = contact_create(nickname,hostname);

						if(connections_connect(newContact, 48691) < 0){
							printf("Failed to estabilish connection.%s", seq);
							pthread_mutex_destroy(&newContact->messageMutex);
							free(newContact);
						}
						else
                        {
                            newContact->status = STATUS_ALIVE;

                            hash_addContact(newContact, nickname);
                            hash_addContact(newContact, hostname);

                            pthread_create(createThread(), 0, message_receive, (void*)newContact);

                            printf("Contact added to the friend list.%s", seq);
                        }
					}
				}
			}

		}
	}

	freeMatrix(args, narg);

	return;
}

void doMsg(char * input, char seq[]){
	int narg;
	char ** args = decompose(input, &narg);

	char * arg1 = NULL;
	char * arg2 = NULL;

	if(narg < 2 || narg > 3){

		printf("Syntax is wrong. Please consult /help if you need to.%s", seq);
	}else{
	    arg1 = args[1];

		contact * target = hash_retrieveContact(arg1);

		if(target == NULL){
			printf("Contact is missing. Review your parameters.%s", seq);
		}else{
			if(target->status == STATUS_DEAD)
				printf("Contact is down. Oh well.%s", seq);
			else{
				if(narg == 2){
					strcpy(messageTarget, arg1);
					printf("%s", seq);
				}else{
				    arg2 = args[2];
					addMessage(target, "You", arg2);

					char * json_msg = makeJSONMessage(arg2);
					message_send(target, json_msg);
					free(json_msg);

					printf("%s", seq);
				}
			}
		}
	}

	freeMatrix(args, narg);

	return;
}

void displayMessages(char * input, char seq[]){
	int narg;
	char ** args = decompose(input, &narg);

	char * arg1 = args[1];

	if(narg != 2){
		printf("Syntax is wrong. Please consult /help.%s", seq);
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

	freeMatrix(args, narg);

	return;
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

							if(contact_by_nick != NULL) {
                                if(contact_by_nick->status == STATUS_ALIVE){
                                    printf("A contact with this name already exists.");
                                }else{
                                    hash_removeContact(contact_by_nick->nickname);
                                    hash_removeContact(contact_by_nick->host_name);

                                    valid = 1;
                                    strcpy(var->nickname, nickname);

                                    var->status = STATUS_ALIVE;

                                    hash_addContact(var, nickname);
                                    // Adiciona o contato na tabela hash usando host_name como chave
                                    hash_addContact(var, var->host_name);

                                    pthread_create(createThread(), 0, message_receive, (void*)var);
                                }
							}else{
								valid = 1;
								strcpy(var->nickname, nickname);

								var->status = STATUS_ALIVE;

								hash_addContact(var, nickname);
								// Adiciona o contato na tabela hash usando host_name como chave
                                hash_addContact(var, var->host_name);

								pthread_create(createThread(), 0, message_receive, (void*)var);
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
				    broadcast_dead();
					stopRunning();
				}else if(cmp(command, "/add")){
					printf("\n");

					addContact(input, seq);
				}else if(cmp(command, "/msg")){
					printf("\n");

					doMsg(input, seq);
				}else if(cmp(command, "/display")){
					printf("\n");

					displayMessages(input, seq);
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
						free(json_msg);

						printf("%s", seq);

						//free(msg);
					}
				}
			}
		}

		free(input);
	}

	return 0;
}
