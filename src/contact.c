#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "contact.h"

contact * contact_create(const char *nickname, const char *host_name)
{
	contact * ret;

    if(ret = (contact*)malloc(sizeof(contact)))
    {
        perror("Error allocating contact");
        return -1;
    }

    strcpy(ret->host_name, host_name);
    strcpy(ret->nickname, nickname);

    ret->references = 0;

    return ret;
}
