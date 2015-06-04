#ifndef _CONTACT_H_
#define _CONTACT_H_

#define STATUS_ALIVE 0
#define STATUS_DEAD 1

typedef struct sContact
{
    unsigned int id;
    int ip;
    char nickname[20];

    int status;
    int lastUpdate;

    int socketvar;
} contact;

#endif