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
		
		fgets(buf, BUFSZ, stdin);
		if(strlen(buf) == 0)
			break;
		char command[COMMANDSZ];
		memset(command, 0, COMMANDSZ);
		
		int _X = -1,_Y = -1;
		
		sscanf(buf,"%s", command);

		if (strcmp(command, "add") == 0 || strcmp(command, "rm") == 0 || strcmp(command, "query") == 0)
			sscanf(buf,"%s %d %d", command, &_X, &_Y);


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
		
			if(count == 0){			
				break;
			}
			
			if(count == 500){
				break;
			}
			if(strchr(buf,'\n')){
				break;
			}
			if(strlen(buf) == 0)
				break;
		}
		
		printf("[msg] from server %s, %d bytes:\n %s", addrstr, (int) count, buf);
		
		if (strlen(buf) == 0){
			printf("2\n");
			break;
		}
	}
	close(s);
	exit(EXIT_SUCCESS);
}