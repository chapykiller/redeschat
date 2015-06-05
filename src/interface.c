#include <stdlib.h>
#include <stdio.h>

#include "global.h"
#include "hashTable.h"

void displayContacts(){
	contact * current;

	for(current = contactList; current!=NULL; current = current->next){
		if(current->status == STATUS_ALIVE)
			printf("%s > %s\n", current->host_name, current->nickname);
	}

	return;
}

int interface_init(){
	char * input = (char *)malloc(256*sizeof(char));
	int i;

	char[] inputSequence = "\n\n\t> ";

	printf("/tYet Another P2P Chat (YAPC)\n");
	printf("\nType /help if you need help.%s", inputSequence);

	while(running){
		fgets(input, 256, stdin);

		int nl = 0;
		for(i=0; input[i]!='\0'; i++)
			if(input[i]=='\n')
				nl = 1;

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
				printf(" - /msg <receiver> <message>\tSends message to receiver without changing chat focus.%s", inputSequence);
			}else if(cmp(command, "/list")){
				printf("\n");

				displayContacts();
			}else if(cmp(command, "/quit")){
				running = 0;
			}else if(cmp(command, "/add")){
				char * hostname = (char *)malloc(31*sizeof(char));
				char * nickname = (char *)malloc(21*sizeof(char));

				int r1 = sscanf(input, "%30s", hostname);
				int r2 = sscanf(input, "%20s", nickname);

				if(r1 == EOF || r2 == EOF){
					printf("")
				}
int connections_connect(contact *newContact, int port)
int contact_create(contact *newContact, char *nickname, char *host_name, int newSocketvar)
			}else if(cmp(command, "/msg")){

			}else if(cmp(command, "/msg")){

			}else if(cmp(command, "/msg")){

			}else{
				printf("\nInvalid command. Type \help if you're lost.%s", inputSequence);
			}

			free(command);
		}else{

		}
	}
}
