#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>          
#include <sys/socket.h>

#include <arpa/inet.h>
#include <unistd.h>


void usage(int argc,char**argv){
	printf("Usage: %s <SERVER IP> <server port>\n",argv[0]);
	printf("example: %s 127.0.0.1 51511\n",argv[0]);
	exit(EXIT_FAILURE);
}


int main(int argc,char**argv){
	if (argc < 3)
		usage(argc, argv);
	struct sockaddr_storage storage;
	if (addrparse(argv[1], argv[2], &storage) != 0 ){
		usage(argc, argv);
	}
	int s = socket(storage.ss_family, SOCK_STREAM, 0);
	if(s == -1 )
			logexit("socket");
	//Se houver um erro o socket retorna -1

	struct sockaddr *addr = (struct sockaddr*)(&storage);
	if (connect(s, addr, sizeof(storage)) != 0 )
		logexit("connect"); 

	char addrstr[BUFSZ];
	addrtostr(addr,addrstr, BUFSZ);
	printf("connected to %s\n", addrstr);

	char buf[BUFSZ];
	while(1){		
	
		
		//inicializar buffer com 0 
		memset(buf, 0, BUFSZ);
		//printf("<mensagem>\n");
		//fflush(stdin);
		fgets(buf, BUFSZ, stdin);
		if(strlen(buf) == 0)
			break;
		char command[COMMANDSZ];
		memset(command, 0, COMMANDSZ);
		
		int _X = -1,_Y = -1;
		//printf("buf: %s",buf);
		
		sscanf(buf,"%s", command);

		if (strcmp(command, "add") == 0 || strcmp(command, "rm") == 0 || strcmp(command, "query") == 0)
			sscanf(buf,"%s %d %d", command, &_X, &_Y);

		sprintf(&buf[strlen(buf)],"\n");

		//numero de bytes
		size_t count = send(s, buf, strlen(buf), 0);
		if (strcmp(command, "kill") == 0 ){
			close(s);
			exit(EXIT_SUCCESS);
		}
		
		if (count != strlen(buf))
			logexit("send");

		memset(buf, 0, BUFSZ);
		unsigned total = 0;
		
		while(1){
			count = recv(s, buf + total,BUFSZ - total,0);
			total += count;
			/* if (count == 0 || strcmp(command, "list") == 0 || strcmp(command, "add") == 0 || strcmp(command, "rm") == 0 || strcmp(command, "query") == 0  || strcmp(command, "kill") == 0 ){	 */
				//conexao terminada 
			printf("ciclo: count = %ld\n",count);
			printf("Size of  buf: %ld\n",strlen(buf));
			if(count == 0){			
				break;
			}
			
			if(count == 500){
				break;
			}
			/* if(count == 5 && strstr(buf,"none\n")){
				break;
			}
			if(count >= 9 && count <= 15 && strstr(buf,"added\n")){
				break;
			}
			if(strstr(buf,"already exists\n")){
				break;
			}
			if(strstr(buf,"removed\n")){
				break;
			}
			if(strstr(buf,"does not exist\n")){
				break;
			} */
			if((strchr(buf,'\n'))){
				break;
			}
			if(strlen(buf) == 0)
				break;
		}
		if (buf[total+1] == '\0'){
			//printf("SIM2\n");
			//buf[strlen(total)+1] = '0';
		}				
		//printf(" received  %u bytes ", total);
		//printf("buf: %s",buf);
		printf("%s",buf);
		//printf("\n");
		/* if (strcmp(command, "kill") == 0 ){
			//for (int k = 0;	k < 999999999;k++){k = k + 1 -1;}
			//conexao terminada 			
			printf("0\n");
			break;
		}
		printf("SIZE of  buf: %ld\n",strlen(buf));
		if (strlen(buf) == 1 && buf[0] == '\n'){
			printf("1\n");
			break;
		}
		if (strlen(buf) == 0){
			printf("2\n");
			break;
		}	 */
		if (strlen(buf) == 0){
			printf("2\n");
			break;
		}
	}
	

	close(s);
	exit(EXIT_SUCCESS);
}