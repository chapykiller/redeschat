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

char ** messageTargets;		// Vetor dos destinatários para mensagem
int targetAmount;			// Tamanho do vetor

int broadcasting;			// Modo broadcast

/*
	Checa se um nome de usuário é válido, isto é, não
	contém caracteres inválidos.
*/
int checkNickname(char * nick){
	int i;

	for(i=0; nick[i]!='\0'; i++){
		// isValidNick é um macro que checa se uma única letra é válida
		if(!isValidNick(nick[i]))
			return 0;
	}

	return 1;
}

/*
	Decompõe a string str em um vetor das strings separadas
	por espaço.
*/
char ** decompose(char * str, int * args){
	int i, j, narg = 0;
	int reading = 0;	// A variável reading diz se uma string válida está sendo lida
	int current = 0;

	// Percorrer a string enquanto não houver quebra de linha ou fim de string.
	// A fim de encontrar o numero de strings totais (narg)
	for(i=0; str[i]!='\n' && str[i]!= '\0'; i++){
		if(str[i] == ' '){
			// Se o caractere for espaço, então pare de ler
			if(reading == 1){
				reading = 0;
			}
		}else{
			// Se o caractere não for espaço, então pode-se
			// começar a ler e incrementar o numero de strings
			// captadas pela função.
			if(reading == 0){
				narg++;
				reading = 1;
			}
		}
	}

	// Alocando um vetor do tamanho conveniente.
	char ** ret = (char **)malloc(narg*sizeof(char *));

	// No começo não se deve ler strings
	reading = 0;

	// Percorrendo novamente a string
	for(i=0; str[i]!='\n' && str[i]!= '\0'; i++){
		if(str[i] == ' '){
			// Se o caractere for espaço, pare de ler.
			if(reading == 1){
				reading = 0;
			}
		}else{
			int size, k;

			if(reading == 0){
				// Se não estivermos lendo mas o caractere atual for válido,
				// isso quer dizer que o caractere anterior era o começo da string
				// ou um espaço, caso no qual a palavra deve ser lida e armazenada
				// no vetor retornado.
				narg++;

				// Conta o tamanho da string até o próximo espaço ou fim de string.
				for(j=i, size=0; str[j]!='\n' && str[j]!= '\0' && str[j]!=' '; j++)
					size++;

				// Aloca-se um vetor do tamanho conveniente.
				ret[current] = (char *)malloc((size+1)*sizeof(char));
				// Armazena-se palavra por palavra da string.
				for(j=i, k=0; str[j]!='\n' && str[j]!= '\0' && str[j]!=' '; j++)
					ret[current][k++] = str[j];
				// Acaba-se com \0.
				ret[current][k] = '\0';

				// Passa-se para a proxima string
				current++;
				reading = 1;
			}

		}
	}

	*args = current;

	return ret;
}

/*
	Apenas libera um vetor de strings (char **)
	de tamanho n.
*/
void freeMatrix(char ** m, int n){
	int i;

	for(i=0; i<n; i++)
		free(m[i]);
	free(m);


	return;
}

/*
	Mostra todos os contatos que o usuário tem.
*/
void displayContacts(char seq[]){
	contact * current;	// Para percorrer lista encadeada.

	// É necessário fazer lock no mutex da lista encadeada de contatos.
    pthread_mutex_lock(&hashMutex);

	if(contactList != NULL){
		int count = 0;

		// Se houver contatos, então deve-se percorrer a lista de contatos
		// e imprimí-los, um a um.
		for(current = contactList; current!=NULL; current = current->next){
			if(current->status == STATUS_ALIVE)
            {
            	// Se o contato está ativo, mostra-o.
				printf(" - %s > %s\n", current->host_name, current->nickname);
				count++;
            }
            else
            {
            	// Se está inativo, mostra-o fazendo a ressalva que está offline.
				printf(" - %s > %s (offline)\n", current->host_name, current->nickname);
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

/*
	Trata um comando do usuário que diz para que um usuário seja adicionado.
*/
void addContact(char * input, char seq[]){
	int narg;

	// Quebra a string em strings menores para que o comando possa ser entendido.
	char ** args = decompose(input, &narg);
	char * hostname = NULL;
	char * nickname = NULL;

	if(narg != 3){
		// Se o comando não tiver 3 palavras, sua semântica está incorreta.
		printf("Syntax is wrong. Please consult /help if you need to.%s", seq);
	}else{
	    hostname = args[1];
        nickname = args[2];

        // Primeiro verifica-se se o nome do usuário pedido é legal.
		if(!checkNickname(nickname)){
			// Se é ilegal, mostra a mensagem de erro apropriada.
			printf("Nickname cotains illegal characters. Please use only letters and numbers.%s", seq);
		}else{
			int length = strlen(nickname);

			// Caso contráriom, deve-se verificar se o nome do usuário
			// está nas especificações crretas de tamanho.
			if(length > 20){
				printf("Nickname is way too long.%s", seq);
			}else if(length == 0){
				printf("Please type a nickname.%s", seq);
			}else{
				// Se estiver, tenta reconhecer algum contato que já tem esse nome
				contact * contact_by_host = hash_retrieveContact(hostname);
				contact * contact_by_nick = hash_retrieveContact(nickname);

				// Se houver um contato ativo com esse hostname, uma mensagem de erro é exibida.
				if(contact_by_host != NULL && contact_by_host->status == STATUS_ALIVE){
					printf("A contact with this hostname already exists.%s", seq);
				}else{
					// Caso contrário, deve-se checar se já existe um usuário ativo com este nickname
					if(contact_by_nick != NULL && contact_by_nick->status == STATUS_ALIVE){
						// Se houver, o pedido do usuário é descartado, já que não se pode ter
						// mais de um usuário com o mesmo nome.

						printf("A contact with this nickname already exists.%s", seq);
					}else{
						// Se já houver um usuário com esse nick ou host mas inativo, remova-o.
						if(contact_by_nick != NULL)
							hash_removeContact(nickname);
						if(contact_by_host != NULL)
							hash_removeContact(hostname);

						// Inicializa um novo contato com os host e nickname pedidos.
						contact * newContact = contact_create(nickname,hostname);

						// Tenta estabelecer conexão.
						if(connections_connect(newContact, 48691) < 0){
							// Se não for possível, mostrar mensagem de erro apropriada.
							printf("Failed to estabilish connection.%s", seq);
							pthread_mutex_destroy(&newContact->messageMutex);
							free(newContact);
						}
						else
                        {
                        	// Se a conexão obtiver sucesso, adicionar o usuário à tabela hash.
                            newContact->status = STATUS_ALIVE;
                            newContact->lastUpdate = time(NULL);

                            hash_addContact(newContact, nickname);
                            hash_addContact(newContact, hostname);

                            // Criando um thread para gerenciar o usuario.
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

/*
	Trata o comando /msg de um usuário.
*/
void doMsg(char * input, char seq[]){
	int narg;
	// Decompõe o comando.
	char ** args = decompose(input, &narg);

	if(narg < 2){
		// Deve haver pelo menos um destinatário para as mensagens.
		printf("Syntax is wrong. Please consult /help if you need to.%s", seq);
	}else{
		int i, j;

		// Aloca um vetor do máximo tamanho possível de destinatários, dado o input
		// do usuário.
		contact ** list = (contact **)malloc((narg-1)*sizeof(contact *));
		int listIndex = 0;

		// Itera sobre todos os destinatários.
		for(i=1; i<narg; i++){
			// Procura o destinatário na tabela hash.
			contact * target = hash_retrieveContact(args[i]);

			if(target == NULL){
				// Se não for encontrado, mostra mensagem de erro.
				printf("%s is missing. Review your parameters.\n", args[i]);
			}else{
				// Se for encontrado, deve-se checar se está ativo.
				if(target->status == STATUS_DEAD)
					// Se estiver inativo, isso é mostrado.
					printf("%s is down. Oh well.\n", args[i]);
				else{
					int hasRepeated = 0;

					// Checa se o destinatário em questão já foi mencionado no mesmo
					// comando.
					for(j=0; j<listIndex; j++){
						if(list[j] == target){
							hasRepeated = 1;
						}
					}

					// Se tiver, o destinatário não é adicionado duas vezes na lista.
					if(!hasRepeated){
						list[listIndex++] = target;
					}
				}
			}
		}

		// Libera a memória dos destinatários anteriores.
		if(messageTargets!=NULL){
			for(i=0; i<targetAmount; i++)
				free(messageTargets[i]);
			free(messageTargets);
		}

		// Aloca uma nova lista de destinatários.
		messageTargets = (char **)malloc(listIndex*sizeof(char *));

		for(i=0; i<listIndex; i++){
			// Adiciona os destinatários apropriados.
			messageTargets[i] = (char *)malloc((strlen(list[i]->nickname)+1)*sizeof(char));
			strcpy(messageTargets[i], list[i]->nickname);
		}

		targetAmount = listIndex;
		broadcasting = 0;	// Se o usuário escolheu destinatários, não está mais em modo broadcast.

		printf("%s", seq);

		free(list);
	}

	// Libera as strings decompostas.

	freeMatrix(args, narg);

	return;
}

/*
	Trata o comando /display
*/
void displayMessages(char * input, char seq[]){
	int narg;
	// Decompõe o input do usuário em strings menores.
	char ** args = decompose(input, &narg);
	
    char * arg1 = NULL;

	if(narg != 2){
		// O comando deve ter exatamente duas strings.
		printf("Syntax is wrong. Please consult /help.%s", seq);
	}else{
	    arg1 = args[1];
		
		// Busca o contato cujas mensagens o usuário quer ver.
        contact * target = hash_retrieveContact(arg1);

		if(target == NULL){
			// Se não foi encontrado, mostra mensagem de erro.
			printf("Contact is missing. Review your parameters.%s", seq);
		}else{
			messageNode * current;

			// Se foi, percorre a lista encadeada até seu final.
			for(current = target->messages; current!=NULL && current->next!=NULL; current = current->next);

			// E depois mostra as mensagens uma a uma, na ordem em que foram guardadas.
			for(; current!=NULL; current = current->prev){
				printf(" || %s\n", current->message);
			}

			if(target->status == STATUS_DEAD)
				// Se o usuário estiver inativo, mostra mensagem
				printf("\nThis contact is no longer available.%s", seq);
			else{
				// Mostra mensagem de sucesso, caso contrário.
				printf("\nMessages displayed.%s", seq);
			}
		}
	}

	// Libera memória das strings alocadas por decompose.

	freeMatrix(args, narg);

	return;
}

/*
	Processa os pedidos de conexão externos.
*/
void processInboundConnections(contact * var){
	if(var == NULL)
		return;

	printf("\n");
	printf("Inbound connections detected. Please give them a nickname.");

	// Para cada pedido de conexão:
	for(; var!=NULL; var = dequeueContact()){
		int valid;

		// Buffer para armazenar nickname.
		char * nickname = (char *)malloc(64*sizeof(char));

		// Rotina para ler, armazenar, adicionar ou rejeitar um contato.
		do{
			// Mostra na tela o hostname da inbound connection e aguarda input
			printf("\n\n Hostname: %s > ", var->host_name);
			fgets(nickname, 64, stdin);

			int nl = 0;
			valid = 0;

			int i;

			// Le a string procurando por quebras de linha. Se houver, nl = 1
			for(i=0; nickname[i]!='\0'; i++){
				if(nickname[i]=='\n')
					nl = 1;
			}

			if(!nl){
				// Se não houver quebra de linha, quer dizer que o input do usuário foi maior
				// que 64 caracteres. O que quer dizer que foi longo demais e que o buffer
				// deve ser esvaziado.

				do{
					// Enquanto não houver quebra de linha, ler mais 64 caracteres.
					fgets(nickname, 64, stdin);

					for(i=0; nickname[i]!='\0'; i++){
						if(nickname[i]=='\n')
							nl = 1;
					}
				}while(!nl);

				printf("Contact name is way too long.");
			}else{
				int length = 0;

				// Conta o tamanho do nickname desejado pelo usuário.
				for(i=0; nickname[i]!='\n'; i++)
					length++;
				nickname[i] = '\0';

				// Se tiver tamanho 0, um erro ocorreu.
				if(length == 0){
					printf("Please type a nickname.");
				}else{
					// Se o tamanho exceder 20 caracteres, é longo demais.
					if(length > 20){
						printf("Contact name is way too long.\n");
					}else{
						// Se tiver caracteres ilegais, o nickname é ilegal.
						if(!checkNickname(nickname)){
							printf("Nickname cotains illegal characters. Please use only letters and numbers.");
						}else{
							// Procura um contato com esse nome.
							contact * contact_by_nick = hash_retrieveContact(nickname);

							if(contact_by_nick != NULL) {
								// Se já existe e está ativo, então caracteriza um erro.
                                if(contact_by_nick->status == STATUS_ALIVE){
                                    printf("A contact with this name already exists.");
                                }else{
                                	// Se ele existe mas está inativo, basta que seja removido
                                	// e o novo contato é adicionado.
                                    hash_removeContact(contact_by_nick->nickname);
                                    hash_removeContact(contact_by_nick->host_name);

                                    valid = 1;
                                    strcpy(var->nickname, nickname);

                                    var->status = STATUS_ALIVE;
                                    var->lastUpdate = time(NULL);

                                    hash_addContact(var, nickname);
                                    // Adiciona o contato na tabela hash usando host_name como chave
                                    hash_addContact(var, var->host_name);

                                    // Cria a thread para administrar o novo usuário.
                                    pthread_create(createThread(), 0, message_receive, (void*)var);
                                }
							}else{
								// Se não houve colisão de nome, basta criar o usuário e inserí-lo na
								// tabela hash.

								valid = 1;
								strcpy(var->nickname, nickname);

								var->status = STATUS_ALIVE;
                                var->lastUpdate = time(NULL);

								hash_addContact(var, nickname);
								// Adiciona o contato na tabela hash usando host_name como chave
                                hash_addContact(var, var->host_name);

                                // Cria a thread para administrar o novo usuário.
								pthread_create(createThread(), 0, message_receive, (void*)var);
							}
						}
					}
				}
			}
		}while(!valid);

		// Libera a memória do buffer.

		free(nickname);
	}

	printf("\n");

	return;
}

int interface_init(){
	int i, j;

	// Sequëncia padrão de caracteres a ser imprimida na tela depois
	// do usuário digitar algo. No caso, duas quebras de linha, um tab
	// e um pequeno ">", indicando que o usuário pode digitar algo.

	char seq[] = "\n\n\t> ";

	// Mostra a mensagem inicial.

	printf("\tYet Another P2P Chat (YAPC)\n");
	printf("\nType /help if you need help.%s", seq);

	messageTargets = NULL;
	broadcasting = 0;
	targetAmount = 0;

	// Enquanto o programa ainda estiver ativo:
	while(isRunning()){
		// Buffer de 513 caracteres para ler input do usuário.
		char * input = (char *)malloc(513*sizeof(char));
		// Lê o input do usuário.
		fgets(input, 512, stdin);

		// Procura por quebras de linha no input.
		int nl = 0;
		for(i=0; input[i]!='\0'; i++)
			if(input[i]=='\n')
				nl = 1;

		if(!nl){
			// Se não houver quebra de linha, quer dizer que o input do usuário foi grande demais,
			// portanto o buffer deve ser esvaziado e a mensagem de erro apropriada será msotrada.

			do{
				// Enquanto não houver quebra de linha no input do usuário, ler mais do stdin.
				nl = 0;
				fgets(input, 512, stdin);

				for(i=0; input[i]!='\0'; i++)
					if(input[i]=='\n')
						nl = 1;
			}while(!nl);

			printf("Your command is way too long. Restrict it to 512 characters.%s", seq);
		}
		else{
			// Substitui a quebra de linha por um \0
			for(i=0; input[i]!='\0'; i++)
				if(input[i]=='\n'){
					input[i] = '\0';
					break;
				}

			// Checa se não houver nenhum pedido de conexão e processa se houver
			processInboundConnections( dequeueContact() );

			if(input[0] == '/'){
				// Se o primeiro caractere do input for uma barra, então é um comando.

				char * command = (char *)malloc(24*sizeof(char));
				sscanf(input, "%23s", command);

				if(cmp(command, "/help")){
					// Se o comando for help, mostra a mensagem apropriada.

					printf("\n");
					printf("Yet Another P2P Chat provides the following commands:\n\n");
					printf(" - /help\t\t\tIt must be pretty obvious what this command does.\n");
					printf(" - /add <hostname> <nickname>\tAdds contact with IP hostname under the name of nickname.\n");
					printf(" - /list\t\t\tLists all your contacts.\n");
					printf(" - /quit\t\t\tQuits the application. :(\n");
					printf(" - /display <contact>\t\tDisplays the last %d messages with contact.\n", MAX_MESSAGES);
					printf(" - /msg <list of recipients>\tAll active contacts on the list will receive messages you send.\n");
					printf(" - /broadcast\tSends message to receiver without changing chat focus.%s", seq);
				}else if(cmp(command, "/list")){
					// Se o comando for /list, mostra os usuários chamando a função displayContacts.
					printf("\n");

					displayContacts(seq);
				}else if(cmp(command, "/quit")){
					// Se o comando for /quit, faz um broadcast a todos os usuários na lista de contatos
					// avisando que o contato ficou inativo.

                    broadcast_dead();
					stopRunning();
				}else if(cmp(command, "/add")){
					// Se o comando for /add, processa o input do usuário de maneira apropriada.
					printf("\n");

					addContact(input, seq);
				}else if(cmp(command, "/msg")){
					// Se o comando for /msg, muda os destinatários das mensagens.
					printf("\n");

					doMsg(input, seq);
				}else if(cmp(command, "/broadcast")){
					// Se o comando for /broadcast, muda o modo de mensagem para broadcast.
					printf("\n");

					broadcasting = 1;
				}else if(cmp(command, "/display")){
					// Se o comando for /list, mostra o histórico de mensagens com o usuário pedido.
					printf("\n");

					displayMessages(input, seq);
				}else{
					printf("\nInvalid command. Type /help if you're lost.%s", seq);
				}

				// Libera a memória do buffer.
				free(command);
			}else{
				// Se o comando não começa com uma barra, então o input do usuário é uma
				// mensagem que deve ser enviada para os destinatários pré-definidos.
				printf("\n");

				if(broadcasting){
				// Se está em modo broadcast, manda mensagem para todos os contatos.
					contact * current;

					// Itera sobre todos os contatos, mandando a mensagem desejada.
					for(current = contactList; current!=NULL; current = current->next){
						if(current->status != STATUS_DEAD){
							// Adiciona a mensagem no histórico de mensagens.
							addMessage(current, "You", input);

							// Encapsula a mensagem usando JSON e envia
							char * json_msg = makeJSONMessage(input);
							message_send(current, json_msg);
							free(json_msg);
						}
					}
				}else{
					// Se não estiver em modo broadcast:

					if(messageTargets == NULL || targetAmount == 0){
						// Se não há nenhum recipiente, o usuário deve definí-los.
						printf("No contact set to message to. Need any /help?");
					}else{
						// Itera sobre os recipientes
						for(i=0; i<targetAmount; i++){
							// Tenta encontrar o contato a partir do nome.
							contact * target = hash_retrieveContact(messageTargets[i]);

							// Marca se o usuário deve ser removido da lista de recipientes.
							int removeFromList = 0;

							if(target == NULL){
								// Se o contato não foi encontrado, então ele deve ser removido da lista de recipientes.
								// O usuário é avisado.

								printf("%s no longer exists.\n", messageTargets[i]);
								removeFromList = 1;
							}else{
								// Se o usuário existe:

								if(target->status == STATUS_DEAD){
									// Se o usuário existe mas está inativo, deve ser removido da lista de contatos.
									printf("%s has disconnected. Message not sent.\n", messageTargets[i]);
									removeFromList = 1;
								}else{
									// Adiciona a mensagem ao histórico de mensagens.
									addMessage(target, "You", input);

									// Encapsula a mensagem usando JSON e envia.
									char * json_msg = makeJSONMessage(input);
									message_send(target, json_msg);
									free(json_msg);
								}
							}

							// Se o usuário foi marcado para remoção da lista de recipientes, então ele será removido.
							if(removeFromList){
								// Depois da posição do contato, copia todos os contatos para uma posição anterior.
								for(j=i+1; j<targetAmount; j++){
									messageTargets[j-1] = messageTargets[j];
								}

								// A ultima posição se torna redundante.
							    targetAmount--;
							}
						}

					}
				}

				// Pede input do usuário.
			    printf("%s", seq);
			}
		}

		// Libera memória do buffer input.
		free(input);
	}

	// Libera a memória alocada ao vetor de recipientes.
    for(i = 0; i < targetAmount; i++)
    {
        free(messageTargets[i]);
    }
    free(messageTargets);

	return 0;
}
