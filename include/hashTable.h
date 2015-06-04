#ifndef _H_HASH_
#define _H_HASH_

#define HASHTABLE_SIZE 128

#define isValid(c) ( ('0' <= (int)c && (int)c <= '9') || c=='.' || ('A' <= (int)c && (int)c <= 'Z') || ('a' <= (int)c && (int)c <= 'z') )

contact * retrieveContact(char * key);

typedef struct sHashNode{
	hashNode * next;

	contact * nodeContact;
} hashNode;

typedef struct sHash{
	hashNode * table;
	int tableSize;

} hashTable;

#endif