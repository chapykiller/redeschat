#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "broadcast.h"
#include "message.h"
#include "contact.h"
#include "running.h"
#include "jsonxstr.h"

/*
 * Envia uma mensagem (ja convertida para JSON) para
 * todos os contatos da lista de amigos
*/
void broadcast_send(char *message)
{
	contact * current; // Utilizado para iterar pela lista de contatos

    // Itera pela lista de contatos
	for(current = contactList; current!=NULL; current = current->next){
		if(current->status == STATUS_ALIVE) // Se o contato estiver online
            message_send(current, message); // Envia a mensagem
	}
}

/*
 * Envia uma mensagem de controle avisando a
 * desconexão para todos os contatos da lista
*/
void broadcast_dead()
{
    char *deadMessage = makeJSONControl(1); // Cria a mensagem de controle para avisar desconexão

    broadcast_send(deadMessage); // Envia a mensagem para todos os contatos

    free(deadMessage); // Libera a memória
}

/*
 * Thread para enviar uma mensagem de controle
 * avisando que permanece conectado a cada 30
 * segundos
*/
void *broadcast_alive(void *data)
{
    time_t last_update;

    char *aliveMessage = makeJSONControl(0); // Cria a mensagem de controle para avisar que esta conectado

    time(&last_update); // Obtem o tempo atual

    // Enquanto o programa estiver ativo
    while(isRunning())
    {
        // Se o tempo atual for menor que o tempo anterior
        if( (time(NULL) - last_update) >= 30)
        {
            broadcast_send(aliveMessage); // Envia a mensagem para todos os contatos

            time(&last_update); // Altera o tempo anterior para o tempo atual
        }

        sleep(1);
    }

    free(aliveMessage); // Libera a memória

    pthread_exit(0); // Finaliza a thread
}
