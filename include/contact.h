#ifndef _CONTACT_H_
#define _CONTACT_H_

#define STATUS_ALIVE 0
#define STATUS_DEAD 1

#include <pthread.h>

#define MAX_MESSAGES 20

struct messageNode;

typedef struct messageNode{
	char * message;

	struct messageNode * next;
	struct messageNode * prev;
}messageNode;

typedef struct sContact
{
    char host_name[31];
    char nickname[21];

    int status;
    int lastUpdate;

    struct sContact * next;
    struct sContact * prev;

    int references;

    messageNode * messages;
    pthread_mutex_t messageMutex;

    int socketvar;
} contact;

typedef struct contactNode{
    contact * value;

    struct contactNode * next;
} contactNode;

contact * contactList;
contactNode * contactQueue;
pthread_mutex_t queueMutex;

contact * contact_create(const char *nickname, const char *host_name);
void addMessage(contact * cont, const char * origin, char * message);

void queueContact(contact * host);
contact * dequeueContact();

void contact_init();
void contact_exit();

#endif
