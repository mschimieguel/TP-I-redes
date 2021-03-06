#pragma once

#include <stdlib.h>
#include <arpa/inet.h>


#define MAXVACSITES 50
#define RESSZ 500
#define COMMANDSZ 6
#define BUFSZ 500

void logexit(const char *msg);
int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage );
void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);
int server_init(const char *proto, const char* portstr , struct sockaddr_storage *storage);