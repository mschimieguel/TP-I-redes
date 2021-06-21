#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSZ 1024

void usage(int argc,char**argv){
	printf("Usage: %s <v4|v6> <server port>\n", argv[0]);
	printf("example: %s v4 51511 \n", argv[0]);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	if (argc < 3)
		usage(argc, argv);

	
	struct sockaddr_storage storage;
	if (server_init(argv[1], argv[2], &storage) != 0 ){
		usage(argc, argv);
	}
	
	
	int s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1 )
		logexit("socket");
	
	//poder usar o mesmo socket sem mudar a porta
	int enable = 1;
    if ( setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0 ) {
        logexit("setsockopt");
    }
	
	//servidor não da connect 
	struct sockaddr *addr = (struct sockaddr*)(&storage);
	
	//bind()
	if (bind(s, addr, sizeof(storage)) !=  0 )
		logexit("bind");	
	//listen()
	
	
	//10 conexoes em espera
	if(listen(s, 10) != 0)
		logexit("listen");	
	//accept()

	
	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);
	printf("bound to %s, waiting connections\n", addrstr);

	while(1){
		//accept retorna um novo socket
		//client_sockaddr recebe de accept o endereco deo client  
		struct sockaddr_storage client_storage;
		struct sockaddr *client_sockaddr = (struct sockaddr*) (&client_storage);
		
		//socket para conversar com o client
		
		socklen_t client_addrlen = sizeof(client_storage);
		int client_socket = accept(s, client_sockaddr,&client_addrlen );  
		if (client_socket == -1)
			logexit("accept");

		char client_addrstr[BUFSZ];
		addrtostr(client_sockaddr, client_addrstr, BUFSZ) ;	
		printf("[Log] connection from %s\n",client_addrstr);

		
		char buf[BUFSZ];
		memset(buf, 0, BUFSZ);
		size_t count = recv(client_socket, buf, BUFSIZ-1, 0);
		printf("[msg] %s, %d bytes: %s\n", client_addrstr, (int) count, buf);
		sprintf(buf,"remote endpoint %.1000s\n", client_addrstr);
		count = send(client_socket, buf, strlen(buf)+1, 0);
		if( count != strlen(buf) + 1 )
			logexit("send");
		close(client_socket);
	}

	exit(EXIT_SUCCESS);
}
