#include <stdlib.h>

#include "hashTable.h"

int cmp(char * s1, char * s2){
	int i;

	for(i=0; s1[i]!=s2[i] && s1[i]!='\0'; i++);
	return s1[i]==s2[i];
}

int getHash(char * key){
	int hash = 0;
	int i;

	int hashchar = 'z' - '.' + 1;

	for(i=0; key[i]!='\0'; i++){
		if(!isValid(key[i])){
			perror("INVALID HASH KEY!");
			return -1;
		}

		hash = (hash * hashChar) % contactTable.tableSize;
		hash = (hash + (key[i] - '.')) % contactTable.tableSize;
	}

	return hash;
}

contact * hash_retrieveContact(char * key){
	int hash = getHash(key);
	if(hash == -1)
		return NULL;

	hashNode * current;

	for(current = contactTable.table[hash]; current!=NULL; current = current.next){
		if(cmp(key, current.key))
			return current;
	}

	return NULL;
}

int hash_addContact(contact * newcontact, char * key){
	int hash = getHash(key);

	hashNode * newNode = (hashNode *)malloc(sizeof(hashNode));
	hashNode.nodeContact = newcontact;
	
	int i, n;
	for(n=0; key[n]!='\0'; n++);

	hashNode.key = (char *)malloc(n*sizeof(char));

	for(i=0; i<n; i++)
		hashNode.key[i] = key[i];

	hashNode.next = contactTable.table[hash];
	contactTable.table[hash] = hashNode;

	return 0;
}

void hash_init(){
	contactTable.tableSize = HASHTABLE_SIZE;
	contactTable.table = (hashNode *)malloc(HASHTABLE_SIZE*sizeof(hashNode));

	int i;

	for(i=0; i<contactTable.tableSize; i++){
		contactTable.table[i] = NULL;
	}

	return;
}