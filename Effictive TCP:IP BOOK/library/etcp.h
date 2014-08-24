#ifndef ETCP_H
#define ETCP_H


#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "skel.h"

extern char *program_name;

int readn(SOCKET fd,char *bp,size_t len);

int readvrec(SOCKET fd,char *bp,size_t len);

void set_address(char *host,char *port,struct sockaddr_in *sap,char *protocol);

void error(int status,int err,char *format,...);

SOCKET tcp_server(char*,char*);

SOCKET tcp_client(char*,char*);

SOCKET udp_server(char*,char*);

SOCKET udp_client(char*,char*);

#endif
