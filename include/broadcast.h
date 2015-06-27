#ifndef _BROADCAST_H_
#define _BROADCAST_H_

/*
 * Envia uma mensagem (ja convertida para JSON) para
 * todos os contatos da lista de amigos
*/
void broadcast_send(char *message);

/*
 * Envia uma mensagem de controle avisando a
 * disconexão para todos os contatos da lista
*/
void broadcast_dead();

/*
 * Thread para enviar uma mensagem de controle
 * avisando que permanece conectado a cada 30
 * segundos
*/
void *broadcast_alive(void *data);

#endif //_BROADCAST_H_
