#include <stdlib.h>
#include <stdio.h>

#include "global.h"
#include "hashTable.h"

char messageTarget[31];

void displayContacts(){
	contact * current;

	if(contactList != NULL){
		for(current = contactList; current!=NULL; current = current->next){
			if(current->status == STATUS_ALIVE)
				printf("%s > %s\n", current->host_name, current->nickname);
		}
	}else{
		printf("Contact list is empty. :(%s", seq);
	}

	return;
}

void addContact(char * input, char * seq){
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

void doMsg(char * input, char * seq){
	char * arg1 = (char *)malloc(31*sizeof(char));
	char * arg2 = (char *)malloc(512*sizeof(char));

	int r1 = sscanf(input, "%30s", hostname);
	int r2 = sscanf(input, "%20s", nickname);

	if(r1 == EOF){
		printf("Syntax is wrong. Please consult /help if you need to.%s", seq);
	}else if(r2 == EOF){
		contact * target = hash_retrieveContact(r1);

		if(target == NULL){
			printf("Contact is missing. Review your parameters.%s", seq);
		}else{
			if(target->status == STATUS_DEAD)
				printf("Contact is down. Oh well.%s", seq);
			else{
				strcpy(messageTarget, target);
			}
		}
	}else{
		char 

		sscanf(input, " %[^\n]",str)
	}
}

int interface_init(){
	int i;

	char[] seq = "\n\n\t> ";

	printf("/tYet Another P2P Chat (YAPC)\n");
	printf("\nType /help if you need help.%s", seq);

	messageTarget = "";

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
					printf("Yet Another P2P Chat provides the following commands:\n\n");
					printf(" - /help\t\t\tIt must be pretty obvious what this command does.\n");
					printf(" - /add <hostname> <nickname>\tAdds contact with IP hostname under the name of nickname.\n");
					printf(" - /list\t\t\tLists all your contacts.\n");
					printf(" - /quit\t\t\tQuits the application. :(\n");
					printf(" - /msg <receiver>\t\tChanges the chat focus to receiver. They will start receiving every message you send.\n");
					printf(" - /msg <receiver> <message>\tSends message to receiver without changing chat focus.%s", seq);
				}else if(cmp(command, "/list")){
					printf("\n");

					displayContacts();
				}else if(cmp(command, "/quit")){
					running = 0;
				}else if(cmp(command, "/add")){
					printf("\n");

					addContact(input);
				}else if(cmp(command, "/msg")){
					printf("\n");

					doMsg(input);
				}else if(cmp(command, "/msg")){

				}else if(cmp(command, "/msg")){

				}else{
					printf("\nInvalid command. Type \help if you're lost.%s", seq);
				}

				free(command);
			}else{

			}
		}

		free(input)
	}
}
