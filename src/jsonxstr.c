#include <stdlib.h>

char * validateJSON(char * arg, int * n){
	int stacksize = 8;
	int stackpos = -1;
	char * stack = (char *)malloc(stacksize*sizeof(char));

	int size = 32;
	char * ret = (char *)malloc(size*sizeof(char));

	int i, j;

	int instring = 0;

	for(i=0; arg[i]!='\0'; i++){
		if(!instring){
			if(arg[i]=='{' || arg[i]=='['){

				if(stackpos++ == stacksize){
					char * newstack = (char *)malloc(stacksize*2*sizeof(char));

					for(j=0; j<stacksize; j++)
						newstack[j] = stack[j];

					stacksize = stacksize*2;
					free(stack);

					stack = newstack;
				}

				stack[stackpos] = arg[j];
			}

			if(i+1>=size){
				char * newret = (char *)malloc(size*2*sizeof(char));

				for(j=0; j<size; j++){
					newret[j] = ret[j];
				}

				size *= 2;
				free(ret);

				ret = newret;
			}

			ret[i] = arg[i];

			if(arg[i]=='}'||arg[i]==']'){
				if(arg[i]!=stack[stackpos])
					perror("JSON HAS ERRORS!");

				stackpos--;

				if(stackpos == -1){
					ret[i+1] = '\0';
				}

				free(stack);
				*n = i+1;

				return ret;
			}
		}
	}

	free(stack);
	free(ret);

	*n = 0;

	return NULL;
}

