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

#define BUFSZ 1024

int main(int argc,char**argv){
	if (argc < 3)
		usage(argc, argv);
	
	struct sockaddr_storage storage;
	if (addrparse(argv[1], argv[2], &storage) != 0 ){
		usage(argc, argv);
	}
	
	int s = socket(storage.ss_family, SOCK_STREAM, 0);
	
	//Se houver um erro o socket retorna -1
	if(s == -1 )
		logexit("socket");
	
	
	struct sockaddr *addr = (struct sockaddr*)(&storage);
	if (connect(s, addr, sizeof(storage)) != 0 )
		logexit("connect"); 
	
	char addrstr[BUFSZ];
	addrtostr(addr,addrstr, BUFSZ);

	printf("connected to %s\n", addrstr);
	

	char buf[BUFSZ];
	//inicializar buffer com 0 
	memset(buf, 0, BUFSZ);
	printf("<mensagem> \n");
	fgets(buf, BUFSZ, stdin);
	
	//numero de bytes
	size_t count = send(s, buf, strlen(buf)+1, 0);
	
	if (count != strlen(buf)+1)
		logexit("send");

	memset(buf, 0, BUFSZ);
	unsigned total = 0;
	while(1){
		count = recv(s, buf + total,BUFSZ - total,0);
		if (count == 0 ){
			//conexao terminada 			
			break;
		}
		total += count;
	}
	close(s);
   
	printf(" received  %u bytes\n", total);
	puts(buf);


	
	exit(EXIT_SUCCESS);
}