#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "hashTable.h"
#include "contact.h"

/*
    Compara duas strings, checando se são iguais.
*/
int cmp(const char * s1, const char * s2)
{
    int i;

    // Percorre posição por posição até que uma das strings acabe ou seu
    // conteúdo seja diferente.
    for(i=0; s1[i]==s2[i] && s1[i]!='\0' && s2[i]!='\0'; i++);

    // Se a posição em que acabou for diferente, então as strings são diferentes.
    return s1[i]==s2[i];
}

/*
    Calcula a função hash para uma chave.
*/
int getHash(char * key)
{
    int hash = 0;
    int i;

    // A base do hash é dado pela quantidade de caracteres que podem ser representados.
    // No caso, todos os caracteres que podem ser representados estão, na tabela ASCII,
    // entre 'z' e '.'. Portanto fazendo sua diferença e acrescentando 1 obteremos o número
    // de caracteres possíveis.
    int hashchar = 'z' - '.' + 1;

    for(i=0; key[i]!='\0'; i++)
    {
        // Se algum caractere for inválido, retornar código de erro -1.
        if(!isValid(key[i]))
            return -1;

        // A chave hash deve ser deslocada na base, ou seja, multiplicada por hashchar
        hash = (hash * hashchar) % contactTable.tableSize;
        // E então somada com o código ASCII normalizado da chave em questão.
        hash = (hash + (key[i] - '.')) % contactTable.tableSize;
    }

    return hash;
}

/*
    A partir de uma chave, busca um contato na tabela hash.
*/
contact * hash_retrieveContact(char * key)
{
    // Calcula a função hash da chave
    int hash = getHash(key);
    if(hash == -1)
        return NULL;

    hashNode * current;

    // Dá lock no mutex da tabela hash.
    pthread_mutex_lock(&hashMutex);

    // Itera sobre os nós da lista ligada da posição calculada da tabela hash
    for(current = contactTable.table[hash]; current!=NULL; current = current->next)
    {
        if(cmp(key, current->key))
        {
            // Se encontra a chave, pode dar unlock no mutex e retornar
            pthread_mutex_unlock(&hashMutex);
            return current->nodeContact;
        }
    }

    // Dá unlock no mutex.
    pthread_mutex_unlock(&hashMutex);

    // Busca falhou.
    return NULL;
}

/*
    Adiciona um novo contato na tabela hash.
*/
int hash_addContact(contact * newcontact, char * key)
{
    // Calcula a função hash
    int hash = getHash(key);

    // Aloca a memória para um novo nõ para a lista ligada da tabela hash
    hashNode * newNode = (hashNode *)malloc(sizeof(hashNode));
    newNode->nodeContact = newcontact;

    // Copia a chave apropriada
    newNode->key = (char *)malloc((strlen(key)+1)*sizeof(char));
    strcpy(newNode->key, key);

    // Dá lock no mutex da tabela hash.
    pthread_mutex_lock(&hashMutex);

    // Adiciona o nó na lista ligada
    newNode->next = contactTable.table[hash];
    contactTable.table[hash] = newNode;

    // Se ainda não há nenhuma referencia ao contato, quer dizer
    // que é a primeira vez que ele é adicionado a tabela hash.
    if(newcontact->references == 0)
    {
        // Portanto ele deve ser adicionado na lista de contatos.

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

    // Incrementa as referencias.
    newcontact->references++;

    // Dá unlock no mutex da tabela hash.
    pthread_mutex_unlock(&hashMutex);

    return 0;
}

/*
    Remove uma certa key da tabela hash
*/
void hash_removeContact(char * key)
{
    // Calcula a função hash da chave pedida
    int hash = getHash(key);
    if(hash == -1)
        return;

    hashNode * current;

    // Dá lock no mutex da tabela hash.
    pthread_mutex_lock(&hashMutex);

    // Percorre cada nó da tabela hah
    for(current = contactTable.table[hash]; current!=NULL && current->next!=NULL; current = current->next)
    {
        // Se a chave da tabela hash for igual a chave buscada
        if(cmp(key, current->next->key))
        {
            // Então remove o nó.
            hashNode * temp = current->next;

            current->next = current->next->next;

            if(--temp->nodeContact->references == 0)
            {
                // Se a quantidade de referências ao contato for 0, então:

                // Muda o próximo contato do contato prévio.
                if(temp->nodeContact->prev != NULL){
                    temp->nodeContact->prev->next = temp->nodeContact->next;
                }

                // Muda o contato prévio do próximo contato.
                if(temp->nodeContact->next != NULL){
                    temp->nodeContact->next->prev = temp->nodeContact->prev;
                }

                // Libera memória
                pthread_mutex_destroy(&temp->nodeContact->messageMutex);
                free(temp->nodeContact);
            }
            free(temp->key);
            free(temp);
        }
    }

    // Checa se o primeiro contato deve ser removido.
    if(contactTable.table[hash] != NULL && cmp(key, contactTable.table[hash]->key))
    {
        hashNode * temp = contactTable.table[hash];
        contactTable.table[hash] = contactTable.table[hash]->next;

        // Se o número de referencias for zero, remove-o.
        if(--temp->nodeContact->references == 0)
        {
            // Se o contato era o primeiro da lista, então o primeiro será
            // o próximo do atual primeiro.
            if(temp->nodeContact->prev != NULL){
                temp->nodeContact->prev->next = temp->nodeContact->next;
            }

            // Se o contato era o primeiro da lista, então o próximo nó não terá nó anterior.
            if(temp->nodeContact->next != NULL){
                temp->nodeContact->next->prev = temp->nodeContact->prev;
            }
            
            pthread_mutex_destroy(&temp->nodeContact->messageMutex);
            free(temp->nodeContact);
        }

        free(temp->key);
        free(temp);
    }

    // Dá unlock no mutex.
    pthread_mutex_unlock(&hashMutex);

    return;
}

/*
    Faz as inicializações necessárias para a tabela hash.
*/
void hash_init()
{
    // Aloca espaço para a tabela.
    contactTable.tableSize = HASHTABLE_SIZE;
    contactTable.table = (hashNode **)malloc(HASHTABLE_SIZE*sizeof(hashNode *));

    int i;

    // Inicializa os ponteiros das listas ligadas, para o caso de colisão.
    for(i=0; i<contactTable.tableSize; i++)
    {
        contactTable.table[i] = NULL;
    }

    contactList = NULL;

    // Inicializa o mutex da tabela hash.
    pthread_mutex_init(&hashMutex, NULL);

    return;
}

/*
    Libera o espaço do que não é mais usado para que a aplicação possa ser fechada.
*/
void hash_exit()
{
    contact *current;
    contact *previous = NULL;

    // Itera sobre cada posição da lista de contatos
    for(current = contactList; current!=NULL; current = current->next)
    {
        // Se previous não for nulo, remove-o da tabela hash, e fecha o socket.
        if(previous != NULL)
        {
            // Fecha o socket
            if(previous->status == STATUS_ALIVE)
                close(previous->socketvar);

            // Remove da tabela hash.
            hash_removeContact(previous->nickname);
            hash_removeContact(previous->host_name);
        }

        previous = current;

        // Delete as mensagens da lista de mensagens.
        messageNode *msgcurrent;
        messageNode *msgprevious = NULL;

        // Percorre a lista
        for(msgcurrent = current->messages; msgcurrent!=NULL; msgcurrent = msgcurrent->next)
        {
            // Delete a mensagem anterior e não a atual, para não perder o ponteiro.
            if(msgprevious != NULL)
            {
                free(msgprevious->message);
                free(msgprevious);
            }

            msgprevious = msgcurrent;
        }

        // Deleta a ultima mensagem, se houver uma.
        if(msgprevious != NULL)
        {
            free(msgprevious->message);
            free(msgprevious);
        }
    }

    // Deleta o último nó da lista de contatos, se houver um.
    if(previous != NULL)
    {
        // Fecha o socket.
        if(previous->status == STATUS_ALIVE)
            close(previous->socketvar);

        // Remove da tabela hash.
        hash_removeContact(previous->nickname);
        hash_removeContact(previous->host_name);
    }

    // Libera a memória da lista de contatos.
    free(contactTable.table);

    pthread_mutex_destroy(&hashMutex);

    return;
}
