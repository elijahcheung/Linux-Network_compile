
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

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

int startThreadPool(int listenSocketfd);

int waitThreadPoolExit();

#endif



