#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <pthread.h>

#include "hashTable.h"
#include "threadManagement.h"

int running;

hashTable contactTable;
contact * contactList;
threadNode * threadList;

pthread_mutex_t hashMutex;

#endif //_GLOBAL_H_
