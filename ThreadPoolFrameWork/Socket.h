#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>


int openSocket(const char *ip,
	uint16_t port);
int openListenSocket(uint16_t port);


ssize_t readN(int fd, void *buf, size_t count);

ssize_t writeN(int fd, const void *buf, 
	size_t count);

ssize_t readLine(int fd, char *buf, 
	size_t count);

#endif
