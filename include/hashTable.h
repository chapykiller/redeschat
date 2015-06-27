#ifndef _H_HASH_
#define _H_HASH_

#include "contact.h"

#define HASHTABLE_SIZE 128 // Tamanho da tabela hash

/*
 * Verifica de uma entrada é valida
*/
#define isValid(c) ( ('0' <= (int)c && (int)c <= '9') || c=='.' || ('A' <= (int)c && (int)c <= 'Z') || ('a' <= (int)c && (int)c <= 'z') )

/*
 * Verifica se um nickname é válido
*/
#define isValidNick(c) ( ('0' <= (int)c && (int)c <= '9') || ('A' <= (int)c && (int)c <= 'Z') || ('a' <= (int)c && (int)c <= 'z') )

pthread_mutex_t hashMutex; // Mutex para acesso à tabela hash

/*
 * Compara duas strings
*/
int cmp(const char * s1, const char * s2);

/*
 * Obtém um hash para uma determinada string(usada como chave)
*/
int getHash(char * key);

/*
 * Obtém um contato a partir do código hash
*/
contact *hash_retrieveContact(char * key);

/*
 * Adiciona um contato na tabela hash
 * usando o apelido ou o hostname
*/
int hash_addContact(contact * newcontact, char * key);

/*
 * Remove um contato da tabela hash
*/
void hash_removeContact(char * key);

/*
 * Inicializa as variáveis necessárias
*/
void hash_init();

/*
 * Libera a memória alocada
*/
void hash_exit();

// Struct utilizada como entrada na tabela hash
typedef struct sHashNode{
	struct sHashNode * next;

	contact * nodeContact; // Contato
	char * key; // Chave (hash) para essa entrada
} hashNode;

// Struct usada como tabela hash
typedef struct sHash{
	hashNode ** table; // Tabela em si
	int tableSize; // Tamanho da tabela

} hashTable;

hashTable contactTable; // Tabela hash

#endif
