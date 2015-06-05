#ifndef _CONTACT_H_
#define _CONTACT_H_

#define STATUS_ALIVE 0
#define STATUS_DEAD 1

typedef struct sContact
{
    char host_name[31];
    char nickname[21];

    int status;
    int lastUpdate;

    struct sContact * next;
    struct sContact * prev;

    int references;

    int socketvar;
} contact;

contact * contact_create(const char *nickname, const char *host_name);

#endif
