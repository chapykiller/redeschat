#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <pthread.h>

#include "hashTable.h"
#include "threadManagement.h"
#include "contact.h"

int running;

hashTable contactTable;
threadNode * threadList;

contact * contactList;
contactNode * contactQueue;

#endif //_GLOBAL_H_
