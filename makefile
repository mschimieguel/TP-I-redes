all:
	gcc -Wall -c list.c
	gcc -Wall -c common.c  
	gcc -Wall client.c common.o -o client
	gcc -Wall server_st.c common.o -o server_st
	gcc -Wall -pthread server.c list.o common.o -o server -lm