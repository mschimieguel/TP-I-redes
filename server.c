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



unsigned thread_exited = 0;

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
	
	char *response = (char*)malloc(RESSZ*sizeof(char));
	memset(response, 0, RESSZ);
	int n_newline = 0;
	
	
	int messages_sizes[100] = {0};
	int response_size = 0;

	for (int i = 0;  i < strlen(buf);i++){
		if (buf[i] == '\n'){
			messages_sizes[n_newline+1] = i;
			n_newline++;			
		}		
	}
	
	for (int i = 0; i < n_newline;i++){
		memset(command, 0, COMMANDSZ);
		
		if(i == 0)
			response_size = -1;
		response_size = strlen(response);
		

		int _X = -1,_Y = -1;
		
		sscanf(&buf[messages_sizes[i]],"%6s", command);
		
				
		if (strcmp(command, "add") == 0 || strcmp(command, "rm") == 0 || strcmp(command, "query") == 0){
			sscanf(&buf[messages_sizes[i]],"%s %d %d", command, &_X, &_Y);
			
			if(_X < 0 || _X > 9999 || _Y < 0 || _Y > 9999 ){
				sprintf(&response[response_size],"position not allowed\n");
				return response;
			}
		}
		else{
			if (strcmp(command, "list") == 0){}
			else {}
		}
		
			
		if(strcmp(command, "add") == 0){
			if (search(vaccine_sites, _X, _Y) == -1){
				if (vaccine_sites->size < MAXVACSITES){
					add_end(vaccine_sites, _X, _Y);
					sprintf(&response[response_size], "%d %d added\n", _X, _Y);
				}
				else{
					sprintf(&response[response_size], "limit exceeded\n");
				}				
			}
			else{
				sprintf(&response[response_size], "%d %d already exists\n" , _X, _Y);
			}				
		}
		else if(strcmp(command, "rm") == 0 ){
			if (remove_element(vaccine_sites, search(vaccine_sites, _X, _Y)) != -1){
				sprintf(&response[response_size], "%d %d removed\n" , _X, _Y);
			}
			else
				sprintf(&response[response_size], "%d %d does not exist\n" , _X, _Y);
		}
		else if( strcmp(command, "list") == 0 ){	
				sprint_list(&response[response_size],vaccine_sites);
		}
		else if (strcmp(command, "query") == 0 ){		
				snearest(response, response_size, vaccine_sites, _X, _Y);			
		}
		else if(strcmp(command, "kill") == 0){}
		else{}
		
	}

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
		int size_response = BUFSZ;	
		memset(buf, 0, BUFSZ);
		size_t count = 0;
		size_t size;
		while(1){
			size = recv(client_data->client_socket, buf + count, BUFSIZ-1 - count, 0);
			count += size;
			if (strchr(buf,'\n')){
				break;
			}			
		}
		printf("[msg] from  client %s, %d bytes:\n %s", client_addrstr, (int) count, buf);
		if (size <= 5 && (strstr(buf, "add") || strstr(buf, "rm") || strstr(buf ,"query") )) {
			printf("ENTROU NO IF\n");
		}
		char *res = commands(buf);
		
		memset(buf, 0, BUFSZ);
		strcpy(buf,res);
		if (strlen(buf) == 0){
			count = send(client_data->client_socket, buf, 1, 0);
			

		}
		memset(res, 0, RESSZ);
		
		for(int i=0; i < BUFSZ; i++){
			if(buf[i] == '\0'){
				size_response--;
			}
		}
		
		count = send(client_data->client_socket, buf, size_response, 0);	
	}
	printf("Deu Close\n");		
    //close(client_data->client_socket);	
	thread_exited++;
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
	//int n_threads = 0;
	while(thread_exited == 0){
		
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
	}
	exit(EXIT_SUCCESS);
}

