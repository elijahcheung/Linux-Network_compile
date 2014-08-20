#include <stdio.h>

#include "Socket.h"
#include "ThreadPool.h"

int main()
{
	int                listenSocketfd = -1;
	int                ret = -1;
	
	listenSocketfd = openListenSocket(5050);
	if (listenSocketfd < 0)
	{
		printf("openListenSocket err\n");
		return -1;
	}

	ret = startThreadPool(listenSocketfd);
	if (ret < 0)
	{
		printf("startThreadPool err");
		close(listenSocketfd);
		return -1;
	}

	waitThreadPoolExit();

	close(listenSocketfd);
	
	
	return 0;
}


