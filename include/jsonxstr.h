#ifndef _H_JSONCONVERT_
#define _H_JSONCONVERT_

#define TYPEMESSAGE 0
#define TYPECONTROL 1

#include "contact.h"

/*
 *  Verifica se o que foi recebido possui alguma
 *  mensagem JSON válida, se sim retorna ela
*/
char * validateJSON(char * arg, int * n);

/*
 * Cria uma mensagem JSON a partir de uma mensage
*/
char * makeJSONMessage(char * string);

/*
 * Cria uma mensagem de controle a partir de um ID
 * de controle
*/
char * makeJSONControl(int id);

/*
 * Transforma uma mensagem JSON em uma
 * mensagem de texto
*/
void decodeJSON(char * message, contact *sender);

#endif
