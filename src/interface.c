

int interface_init(){
	char * input = (char *)malloc(256*sizeof(char));
	int i;

	printf("Yet Another P2P Chat (YAPC)\n")

	while(running){
		fgets(input, 256, stdin);

		int nl = 0;
		for(i=0; input[i]!='\0'; i++)
			if(input[i]=='\n')
				nl = 1;

		if(input[0] == '/'){
			
		}else{

		}
	}
}