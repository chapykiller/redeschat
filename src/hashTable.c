#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "hashTable.h"
#include "contact.h"

int cmp(const char * s1, const char * s2)
{
    int i;

    for(i=0; s1[i]==s2[i] && s1[i]!='\0' && s2[i]!='\0'; i++);
    return s1[i]==s2[i];
}

int getHash(char * key)
{
    int hash = 0;
    int i;

    int hashchar = 'z' - '.' + 1;

    for(i=0; key[i]!='\0'; i++)
    {
        if(!isValid(key[i]))
            return -1;

        hash = (hash * hashchar) % contactTable.tableSize;
        hash = (hash + (key[i] - '.')) % contactTable.tableSize;
    }

    return hash;
}

contact * hash_retrieveContact(char * key)
{
    int hash = getHash(key);
    if(hash == -1)
        return NULL;

    hashNode * current;

    pthread_mutex_lock(&hashMutex);

    for(current = contactTable.table[hash]; current!=NULL; current = current->next)
    {
        if(cmp(key, current->key))
        {
            pthread_mutex_unlock(&hashMutex);
            return current->nodeContact;
        }
    }

    pthread_mutex_unlock(&hashMutex);

    return NULL;
}

int hash_addContact(contact * newcontact, char * key)
{
    int hash = getHash(key);

    hashNode * newNode = (hashNode *)malloc(sizeof(hashNode));
    newNode->nodeContact = newcontact;

    newNode->key = (char *)malloc((strlen(key)+1)*sizeof(char));

    strcpy(newNode->key, key);

    pthread_mutex_lock(&hashMutex);

    newNode->next = contactTable.table[hash];
    contactTable.table[hash] = newNode;

    if(newcontact->references == 0)
    {
        if(contactList == NULL)
        {
            contactList = newcontact;

            newcontact->next = NULL;
            newcontact->prev = NULL;
        }
        else
        {
            contactList->prev = newcontact;

            newcontact->next = contactList;
            newcontact->prev = NULL;

            contactList = newcontact;
        }
    }

    newcontact->references++;

    pthread_mutex_unlock(&hashMutex);

    return 0;
}

void hash_removeContact(char * key)
{
    int hash = getHash(key);
    if(hash == -1)
        return;

    hashNode * current;

    pthread_mutex_lock(&hashMutex);

    for(current = contactTable.table[hash]; current!=NULL && current->next!=NULL; current = current->next)
    {
        if(cmp(key, current->next->key))
        {
            hashNode * temp = current->next;

            current->next = current->next->next;

            if(--temp->nodeContact->references == 0)
            {
                if(temp->nodeContact->prev != NULL){
                    temp->nodeContact->prev->next = temp->nodeContact->next;
                }

                if(temp->nodeContact->next != NULL){
                    temp->nodeContact->next->prev = temp->nodeContact->prev;
                }

                pthread_mutex_destroy(&temp->nodeContact->messageMutex);
                free(temp->nodeContact);
            }
            free(temp->key);
            free(temp);
        }
    }

    if(cmp(key, contactTable.table[hash]->key))
    {
        hashNode * temp = contactTable.table[hash]->next;

        if(--contactTable.table[hash]->nodeContact->references == 0)
        {
            if(contactTable.table[hash]->nodeContact->prev != NULL){
                contactTable.table[hash]->nodeContact->prev->next = contactTable.table[hash]->nodeContact->next;
            }

            if(contactTable.table[hash]->nodeContact->next != NULL){
                contactTable.table[hash]->nodeContact->next->prev = contactTable.table[hash]->nodeContact->prev;
            }
            
            pthread_mutex_destroy(&contactTable.table[hash]->nodeContact->messageMutex);
            free(contactTable.table[hash]->nodeContact);
        }

        free(contactTable.table[hash]->key);
        free(contactTable.table[hash]);

        contactTable.table[hash] = temp;
    }

    pthread_mutex_unlock(&hashMutex);

    return;
}

void hash_init()
{
    contactTable.tableSize = HASHTABLE_SIZE;
    contactTable.table = (hashNode **)malloc(HASHTABLE_SIZE*sizeof(hashNode *));

    int i;

    for(i=0; i<contactTable.tableSize; i++)
    {
        contactTable.table[i] = NULL;
    }

    contactList = NULL;

    pthread_mutex_init(&hashMutex, NULL);

    return;
}

void hash_exit()
{
    contact *current;
    contact *previous = NULL;
    for(current = contactList; current!=NULL; current = current->next)
    {
        if(previous != NULL)
        {
            if(previous->status == STATUS_ALIVE)
                close(previous->socketvar);

            hash_removeContact(previous->nickname);
            hash_removeContact(previous->host_name);
        }

        previous = current;

        messageNode *msgcurrent;
        messageNode *msgprevious = NULL;
        for(msgcurrent = current->messages; msgcurrent!=NULL; msgcurrent = msgcurrent->next)
        {
            if(msgprevious != NULL)
            {
                free(msgprevious->message);
                free(msgprevious);
            }

            msgprevious = msgcurrent;
        }

        if(msgprevious != NULL)
        {
            free(msgprevious->message);
            free(msgprevious);
        }
    }
    if(previous != NULL)
    {
        if(previous->status == STATUS_ALIVE)
            close(previous->socketvar);

        hash_removeContact(previous->nickname);
        hash_removeContact(previous->host_name);
    }

    free(contactTable.table);

    pthread_mutex_destroy(&hashMutex);

    return;
}
