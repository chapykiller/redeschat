#ifndef _INTERFACE_H_
#define _INTERFACE_H_

void displayContacts(char seq[]);
void addContact(char * input, char seq[]);
void doMsg(char * input, char seq[]);
void displayMessages(char * input, char seq[]);
int interface_init();

#endif //_INTERFACE_H_
