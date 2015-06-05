#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <pthread.h>

#include "hashTable.h"
#include "threadManagement.h"

int running;

hashTable contactTable;
contact * contactList;
threadNode * threadList;

#endif //_GLOBAL_H_
