#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "contact.h"

int contact_create(contact *newContact, char *nickname, char *host_name, int newSocketvar)
{
    if(newContact = (contact*)malloc(sizeof(contact)))
    {
        perror("Error allocating contact");
        return -1;
    }

    strcpy(newContact->host_name, host_name);
    strcpy(newContact->nickname, nickname);

    newContact->socketvar = newSocketvar;

    return 0;
}
