#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <jansson.h>
#include <string.h>

#include "jsonxstr.h"
#include "contact.h"

/*
 *  Verifica se o que foi recebido possui alguma
 *  mensagem JSON válida, se sim retorna ela
*/
char * validateJSON(char * arg, int * n){
	int stacksize = 8; // Tamanho inicial da pilha
	int stackpos = -1;
	char * stack = (char *)malloc(stacksize*sizeof(char)); // Aloca uma pilha de '[' e '{'

	int size = 32; // Tamanho maximo do buffer ret
	char * ret = (char *)malloc(size*sizeof(char)); // Aloca o espaço para colocar a mensagem decodificada

	int i, j;

	int instring = 0; // Indica se o que vem a seguir é texto dentro de "" ou se é parte do JSON

    // Itera do começo até o final da mensagem
	for(i=0; arg[i]!='\0'; i++){
        // Se encontrou o indicio que o que vem a seguir é inicio ou final de texto dentro de ""
		if(arg[i]=='"' && (i == 0 || arg[i-1]!='\\')){
			instring = !instring; // O valor de instring é alternado
		}

        // Se execedeu o tamanho do buffer
		if(i+1>=size){
            // Aloca mais espaço
			char * newret = (char *)malloc(size*2*sizeof(char));

			for(j=0; j<size; j++){
				newret[j] = ret[j];
			}

			size *= 2;
			free(ret);

			ret = newret;
		}

		ret[i] = arg[i];

        // Se não for um texto dentro de ""
		if(!instring){
            // Verifica se é um { ou [
			if(arg[i]=='{' || arg[i]=='['){

                // Se execedeu o tamanho da pilha
				if(stackpos++ == stacksize){
                    // Alo9ca mais espaço
					char * newstack = (char *)malloc(stacksize*2*sizeof(char));

					for(j=0; j<stacksize; j++)
						newstack[j] = stack[j];

					stacksize = stacksize*2;
					free(stack);

					stack = newstack;
				}

                // Coloca esse caracter na pilha
				stack[stackpos] = arg[i];
            }

			// Se encontrar um } ou ]
            if(arg[i]=='}'||arg[i]==']'){
                // Se for }
                if(arg[i] == '}'){
                    // Verifica se a ultima posicao da pilha não é um { pra ser fechado
                    if(stack[stackpos] != '{')
                        // Ocorreu erro
                        printf("JSON HAS ERRORS!");
                }
                else{ // Se for ]
                    // Verifica se a ultima posição da pilha não é um [ pra ser fechado
                    if(stack[stackpos] != '[')
                        printf("JSON HAS ERRORS!");
                }

                // Diminui a pilha
				stackpos--;

				// Se nao possui mais nada na pilha
                if(stackpos == -1){
					ret[i+1] = '\0'; // Indica final de string
                
                    // Libera a memória da pilha
                    free(stack);
				    *n = i+1;

				    return ret;
				}

			}
		}
	}

	free(stack);
	free(ret);

	*n = 0;

	return NULL;
}

/*
 * Cria uma mensagem JSON a partir de
 * uma mensagem de texto
*/
char * makeJSONMessage(char * string){
	json_t * array = json_array(); // Cria um array JSON

	json_t * type_j = json_integer(TYPEMESSAGE); // Indica que o tipo é mensagem de texto
	json_t * messageType_j = json_string(string); // Coloca a mensagem de texto

    // Coloca as informações anterior na mensagem JSON final
	json_array_append_new(array, type_j);
	json_array_append_new(array, messageType_j);

    // Obtem a mensagem JSON
	char *message = json_dumps(array, 0);

    // Libera memória
    json_decref(array);

    // Retorna a mensagem JSON
    return message;
}

/*
 * Cria uma mensagem de controle a partir de um ID
 * de controle
*/
char * makeJSONControl(int id){
	json_t * array = json_array(); // Cria um array JSON

	json_t * type_j = json_integer(TYPECONTROL); // Indica que o tipo é mensagem de controle
	json_t * controlType_j = json_integer(id); // Indica qual é o ID de controle

    // Coloca as informações anteriores na mensagem JSON final
	json_array_append_new(array, type_j);
	json_array_append_new(array, controlType_j);

    // Obtém a mensagem JSON
	char* control = json_dumps(array, 0);

    // Libera memória
    json_decref(array);

    // Retorna a mensagem de controle
    return control;
}

/*
 * Transforma uma mensagem JSON em uma
 * mensagem de texto
*/
void decodeJSON(char * message, contact *sender){
	json_t * root;
	json_error_t error;

	// Carrega a mensagem
    root = json_loads(message, 0, &error);

	// Obtem o tipo da mensagem (texto ou controle)
    json_t * type_j = json_array_get(root, 0);
	int type = json_integer_value(type_j);

    // Se for de texto
	if(type == TYPEMESSAGE){
        // Coloca a mensagem no buffer do contato correspondente
        char message[512];

        strcpy(message, json_string_value(json_array_get(root, 1)));
        
        addMessage(sender, sender->nickname, message);
	}
    // Se for de controle
    else if(type == TYPECONTROL) {
        // Obtem o ID de controle
        int controlType = json_integer_value(json_array_get(root, 1));

        if(controlType == 1) // Se for uma mensagem avisando desconexão
            sender->status = STATUS_DEAD;
	}else{
		printf("MESSAGE WITH INVALID TYPE!");
	}

    // Libera memória
    json_decref(root);
}
