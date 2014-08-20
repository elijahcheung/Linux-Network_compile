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

#include "Socket.h"

void *sndPacketThread(void *arg)
{
	int     socketfd = -1;
	char    sndBuf[100];
	int     ret = -1;

	socketfd = *((int *)arg);
	free(arg);
	
	while (1)
	{
		ret = readLine(0, sndBuf, sizeof(sndBuf));//标准输入文件到数组
		if (ret < 0)
		{
			perror("readLine err");
			break;
		}
		else if (ret == 0)
		{
			shutdown(socketfd, SHUT_WR);//破坏了socket 链接，读的时候
			//可能侦探到EOF结束符
			break;
		}

		ret = writeN(socketfd, sndBuf, ret);
		if (ret < 0)
		{
			perror("writeN err");
			break;
		}
	}

	return NULL;
}

void *rcvPacketThread(void *arg)
{
	int     socketfd = -1;
	char    rcvBuf[100];
	int     ret = -1;

	socketfd = *((int *)arg);
	free(arg);
	
	while (1)
	{
		ret = readLine(socketfd, rcvBuf, sizeof(rcvBuf));
		if (ret < 0)
		{
			perror("readLine err");
			break;
		}
		else if (ret == 0)//当没有从socket读到数据
		{
			printf("Server end\n");
			shutdown(socketfd, SHUT_RD);
			break;
		}

		ret = writeN(1, rcvBuf, ret);//writeN中的1代表标准输出（屏幕）
		if (ret < 0)
		{
			perror("writeN err");
			break;
		}
	}

	return NULL;
}


int main()
{
	pthread_t          tid[2];
	int                socketfd = -1;
	int                *psocketfd = NULL;
	int                ret = -1;
	int                i = 0;


	socketfd = openSocket("127.0.0.1", 5050);  //
	if (socketfd < 0)
	{
		printf("openSocket err");
		return -1;
	}
	
	psocketfd = (int *)malloc(sizeof(int));
	*psocketfd = socketfd;
	ret = pthread_create(&tid[0], NULL, sndPacketThread, psocketfd);
	if (ret != 0)
	{
		perror("pthread_create err");
		close(socketfd);
		return -1;
	}

	psocketfd = (int *)malloc(sizeof(int));
	*psocketfd = socketfd;
	ret = pthread_create(&tid[1], NULL, rcvPacketThread, psocketfd);
	if (ret != 0)
	{
		perror("pthread_create err");
		close(socketfd);
		return -1;
	}

	for (i=0; i<2; i++)
	{
		ret = pthread_join(tid[i], NULL);
		if (ret != 0)
		{
			perror("pthread_join err");
			close(socketfd);
			return -1;
		}

	}

	return 0;
}


