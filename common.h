#pragma once

#include <stdlib.h>
#include <arpa/inet.h>


#define MAXVACSITES 50
#define RESSZ 100
#define COMMANDSZ 5
#define BUFSZ 1024

void logexit(const char *msg);
int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage );
void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);
int server_init(const char *proto, const char* portstr , struct sockaddr_storage *storage);