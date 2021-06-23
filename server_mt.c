#include "common.h"
#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>



list_t *vaccine_sites;


void usage(int argc,char**argv){
	printf("Usage: %s <v4|v6> <server port>\n", argv[0]);
	printf("example: %s v4 51511 \n", argv[0]);
	exit(EXIT_FAILURE);
}

struct client_data{
    int client_socket;
    struct sockaddr_storage client_storage;
};
char* commands(char* buf){	
	
	char command[COMMANDSZ];
	memset(command, 0, COMMANDSZ);
	char *response = (char*)malloc(RESSZ*sizeof(char));
	memset(response, 0, RESSZ);
	
	
	int _X = -1,_Y = -1;
	//printf("buf: %s\n",buf);
	sscanf(buf,"%s", command);
	
	
	if (strcmp(command, "add") == 0 || strcmp(command, "rm") == 0 || strcmp(command, "query") == 0){
		sscanf(buf,"%s %d %d", command, &_X, &_Y);
		if(_X < 0 || _X > 9999 || _Y < 0 || _Y > 9999 ){
			sprintf(response,"position not allowed");
			print_list(vaccine_sites);
			return response;
		}
	}
	else{
		if (strcmp(command, "list") == 0){}
		else {
			//sprintf(response,"command not allowed");
			//print_list(vaccine_sites);
			//return response;
		}
	}
	
		
	if(strcmp(command, "add") == 0){
		if (search(vaccine_sites, _X, _Y) == -1){
			if (vaccine_sites->size < MAXVACSITES){
				add_end(vaccine_sites, _X, _Y);
				sprintf(response, "%d %d added", _X, _Y);
			}
			else
				sprintf(response, "limit exceeded");
		}
		else			
			sprintf(response, "%d %d already exists" , _X, _Y);
	}
	else if(strcmp(command, "rm") == 0 ){
		if (remove_element(vaccine_sites, search(vaccine_sites, _X, _Y)) != -1){
			sprintf(response, "%d %d removed" , _X, _Y);
		}
		else
			sprintf(response, "%d %d does not exist" , _X, _Y);
	}
	else if( strcmp(command, "list") == 0 ){
		sprint_list(response,vaccine_sites);
	}
	else if (strcmp(command, "query") == 0 ){
		snearest(response, vaccine_sites, _X, _Y);
	}
	else if(strcmp(command, "kill") == 0){
		//printf("entrou no if kill\n");
		//response = (char*)malloc(5*sizeof(char));
		sprintf(response,"kill");
	}
	else{}


	print_list(vaccine_sites);
	//printf("response: %s\n",response);
	
	return response;
}
 
void* client_thread(void *data){
    struct client_data *client_data = (struct client_data *)data;
    struct sockaddr *client_sockaddr = (struct sockaddr *) (&client_data->client_storage);


    char client_addrstr[BUFSZ];
    addrtostr(client_sockaddr, client_addrstr, BUFSZ) ;	
    printf("[Log] connection from %s\n",client_addrstr);


    char buf[BUFSZ];
	//while para poder criar as threads
	while(1){
			
		memset(buf, 0, BUFSZ);
		size_t count = recv(client_data->client_socket, buf, BUFSIZ-1, 0);
		
		
		char *res = commands(buf);
		printf("client_thread response: %s\n",res);
		
		
		printf("[msg] %s, %d bytes: %s\n ", client_addrstr, (int) count, buf);
		strcpy(buf,res);
		memset(res, 0, RESSZ);
		//sprintf(buf,"remote endpoint %.1000s\n", client_addrstr);
		//sprintf(buf,"\n",);
		//count = send(client_data->client_socket, buf, strlen(buf)+1, 0);
		count = send(client_data->client_socket, buf, strlen(buf)+1, 0);
		if( count != strlen(buf) + 1 )
			logexit("send");
		//printf("fim while thread\n");	
	}		
    close(client_data->client_socket);
	//printf("quebrou while client_thread\n");
	//printf("posfim while thread\n");	
    pthread_exit(EXIT_SUCCESS);
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

	vaccine_sites = create_list();
	int n_threads = 0;
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
		
        struct client_data *cdata = malloc(sizeof(*cdata));

        if(!cdata){
            logexit("malloc");
        }
        cdata->client_socket = client_socket;
        memcpy(&(cdata->client_storage), &client_storage, sizeof(client_storage));

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);

		printf("n_threads: %d\n",n_threads);
		n_threads++;
	}
	exit(EXIT_SUCCESS);
}

