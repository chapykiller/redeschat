#ifndef _H_HASH_
#define _H_HASH_

#include "contact.h"

#define HASHTABLE_SIZE 128

#define isValid(c) ( ('0' <= (int)c && (int)c <= '9') || c=='.' || ('A' <= (int)c && (int)c <= 'Z') || ('a' <= (int)c && (int)c <= 'z') )


int cmp(const char * s1, const char * s2);
int getHash(char * key);

contact *hash_retrieveContact(char * key);
int hash_addContact(contact * newcontact, char * key);

void hash_init();


struct sHashNode;

typedef struct sHashNode{
	struct sHashNode * next;

	contact * nodeContact;
	char * key;
} hashNode;

typedef struct sHash{
	hashNode ** table;
	int tableSize;

} hashTable;

#endif
