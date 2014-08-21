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

static void set_address(char *host,char *port,struct sockaddr_in *sap,char *protocol);

SOCKET tcp_server(char*,char*);

SOCKET tcp_client(char*,char*);

#endif
