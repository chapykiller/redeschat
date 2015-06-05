#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "contact.h"

#define MESSAGE_MAXSIZE 519

/*
 * Envia uma mensagem já codificada utilizando json
 * para um contato especifico
*/
int message_send(contact *receiver, char *send_data);

/*
 * Thread responsavel por receber mensagens de outros
 * contatos
 * (void*)data é do tipo contact*
*/
void *message_receive(void *data);

#endif //_MESSAGE_H_
