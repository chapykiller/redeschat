#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <pthread.h>

#include "hashTable.h"

int running;

hashTable contactTable;
contact * contactList;

pthread_mutex_t hashMutex;

#endif //_GLOBAL_H_
