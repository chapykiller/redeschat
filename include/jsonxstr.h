#ifndef _H_JSONCONVERT_
#define _H_JSONCONVERT_

#define TYPEMESSAGE 0
#define TYPECONTROL 1

#include "contact.h"

char * validateJSON(char * arg, int * n);

char * makeJSONMessage(char * string);
char * makeJSONControl(int id);

void decodeJSON(char * message, contact *sender);

#endif
