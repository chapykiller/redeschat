#ifndef _INTERFACE_H_
#define _INTERFACE_H_

/*
 * Exibe a lista de contatos
*/
void displayContacts(char seq[]);

/*
 * Adiciona um contato para a lista de contatos
 * do usuário
*/
void addContact(char * input, char seq[]);

/*
 * Realiza os procedimentos necessários para
 * enviar uma mensagem
*/
void doMsg(char * input, char seq[]);

/*
 * Exibe as ultimas mensagens da conversa
 * com o usuário em questão
*/
void displayMessages(char * input, char seq[]);

/*
 * Inicializa a interface
*/
int interface_init();

#endif //_INTERFACE_H_
