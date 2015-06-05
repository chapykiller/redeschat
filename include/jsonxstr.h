#ifndef _H_JSONCONVERT_
#define _H_JSONCONVERT_

#define TYPEMESSAGE 0
#define TYPECONTROL 1

char * validateJSON(char * arg, int * n);

char * makeJSONMessage(char * string);
char * makeJSONControl(int id);

void decodeJSON(char * message);

#endif