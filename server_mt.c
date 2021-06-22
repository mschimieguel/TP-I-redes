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
char *response;
int alive = 1;

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
	response = (char*)malloc(RESSZ*sizeof(char));
	memset(response, 0, RESSZ);
	
	
	int _X = -1,_Y = -1;
	//printf("buf: %s\n",buf);
	sscanf(buf,"%s", command);
	
	
	if (strcmp(command, "add") == 0 || strcmp(command, "rm") == 0 || strcmp(command, "query") == 0)
		sscanf(buf,"%s %d %d", command, &_X, &_Y);
	
	if(strcmp(command, "add") == 0){
		if (search(vaccine_sites, _X, _Y) == -1 && vaccine_sites->size < MAXVACSITES){
			add_end(vaccine_sites, _X, _Y);
			sprintf(response, "%d %d added" , _X, _Y);
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
		response = (char*)malloc(5*sizeof(char));
		response[0] = 'k';response[1] = 'i';response[2] = 'l';response[3] = 'l';response[4] = '\0';
		alive = 0;
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
	
	while(1){
			
		memset(buf, 0, BUFSZ);
		size_t count = recv(client_data->client_socket, buf, BUFSIZ-1, 0);
		
		
		response = commands(buf);
		//printf("client_thread response: %s\n",response);
		if (strcmp(response, "kill" ) == 0){
			//printf("client_thread if\n");
			alive = 0;
			break;
		}
		
		printf("[msg] %s, %d bytes: %s\n ", client_addrstr, (int) count, buf);
		strcpy(buf,response);
		memset(response, 0, RESSZ);
		//sprintf(buf,"remote endpoint %.1000s\n", client_addrstr);
		//sprintf(buf,"\n",);
		//count = send(client_data->client_socket, buf, strlen(buf)+1, 0);
		count = send(client_data->client_socket, buf, strlen(buf)+1, 0);
		if( count != strlen(buf) + 1 )
			logexit("send");
	}		
    close(client_data->client_socket);
	//printf("quebrou while client_thread\n");
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

	while(alive){
		
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
		printf("response: %s\n", response);
		printf("alive: %d\n",alive);
		//printf("strcmp: %d\n", strcmp(response,"kill"));
		if (response && strstr(response,"kill") == 0){
			printf("main if\n");
			break;			
		}
	}
	exit(EXIT_SUCCESS);
}

