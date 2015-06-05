#ifndef _H_HASH_
#define _H_HASH_

#include "contact.h"

#define HASHTABLE_SIZE 128

#define isValid(c) ( ('0' <= (int)c && (int)c <= '9') || c=='.' || ('A' <= (int)c && (int)c <= 'Z') || ('a' <= (int)c && (int)c <= 'z') )

contact *hash_retrieveContact(char * key);

int hash_addContact(contact * newcontact, char * key);

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
