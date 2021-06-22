all:
	gcc -Wall -c list.c
	gcc -Wall -c common.c  
	gcc -Wall client.c common.o -o client
	gcc -Wall server.c common.o -o server
	gcc -Wall -pthread server_mt.c list.o common.o -o server_mt -lm