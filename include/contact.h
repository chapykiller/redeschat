#ifndef _CONTACT_H_
#define _CONTACT_H_

#define STATUS_ALIVE 0
#define STATUS_DEAD 1

typedef struct sContact
{
    char host_name[30];
    char nickname[20];

    int status;
    int lastUpdate;

    contact * next;
    contact * prev;

    int socketvar;
} contact;

int contact_create(contact *newContact, char *nickname, char *host_name, int newSocketvar);

#endif
