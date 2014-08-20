
#include <stdint.h>
#include "ThreadPool.h"

#define THREAD_MAX_SIZE    1024   //最大线程数量1024
#define THREAD_INIT_SIZE   15 	//线程初始化数量15

#define THREAD_ALERT_LINE1 10 //线程空闲数警戒线
#define THREAD_ALERT_LINE2 20 //线程空闲数量是否过多

/*
线程池结构体
*/
typedef struct st_thread_pool
{

	int               listenSocketfd;
	pthread_mutex_t   mutex;
	pthread_t         manageTid;  //管理线程
	pthread_t         tid[THREAD_MAX_SIZE];//当前使用线程数量
	uint8_t           idle[THREAD_MAX_SIZE];//空闲线程
	int               size;
} ST_THREAD_POOL;


static ST_THREAD_POOL  threadPool;

static void *threadFun(void *arg);
static void *threadPoolManageThread(void *arg);

static int setThreadIdle(pthread_t tid, uint8_t flag);

static int addThread(int n);
static int subThread(int n);

/*
函数功能：线程池开始
形参：   套接字
*/
int startThreadPool(int listenSocketfd)
{
	int     i = 0;
	int     ret = -1;

	threadPool.listenSocketfd = listenSocketfd;
	
	ret = pthread_mutex_init(&(threadPool.mutex),NULL);//初始化互斥锁
	if (ret != 0)
	{

		return -1;
	}

	threadPool.size = THREAD_INIT_SIZE;//线程数量初始化15
	for (i=0; i<THREAD_INIT_SIZE; i++)//创建线程
	{	
		threadPool.idle[i] = 1;	
		ret = pthread_create(&(threadPool.tid[i]), NULL, threadFun, NULL);
		if (ret != 0)
		{
			perror("pthread_create err");
			return -1;
		}

	}

	ret =pthread_create(&(threadPool.manageTid),NULL, threadPoolManageThread, NULL);//管理线程创建
	if (ret != 0)
	{
		perror("pthread_create err");
		return -1;
	}
	

	return 0;
}


/*
函数功能：线程退出
*/
int waitThreadPoolExit()
{
	int     i = 0;
	int     ret = -1;

	ret = pthread_join((threadPool.manageTid), NULL);
	if (ret != 0)
	{
		perror("pthread_join err");
		return -1;
	}

	return 0;
}

/*
函数功能：线程链接后，并读取数据
*/

static void *threadFun(void *arg)
{
	struct sockaddr_in clientAddr;
	socklen_t          clientAddrLen;
	int                socketfd;
	char               rcvBuf[100];
	int                ret = -1;

	while (1)
	{
		setThreadIdle(pthread_self(),1);//获取当前线程标识符

		pthread_mutex_lock(&(threadPool.mutex));
		clientAddrLen = sizeof(clientAddr);
		
		socketfd=accept(threadPool.listenSocketfd, 
				(struct sockaddr *)&clientAddr,
				&clientAddrLen);
		
		if (socketfd < 0)
		{
			perror("accept err");
			pthread_mutex_unlock(&(threadPool.mutex));
			continue;//链接失败时，重新链接
		}
		
		setThreadIdle(pthread_self(), 0);
		//如果有链接套接字则设置为忙状态
		
		pthread_mutex_unlock(&(threadPool.mutex));
	
		while (1)
		{
			ret = readLine(socketfd, rcvBuf, sizeof(rcvBuf));
			if (ret < 0)
			{
				perror("readLine err");
				close(socketfd);
				break;
			}
			else if (ret == 0)
			{
				printf("clien end\n");
				close(socketfd);
				break;
			}

			ret = writeN(socketfd, 
				rcvBuf, ret);
			if (ret < 0)
			{
				perror("writeN err");
				close(socketfd);
				break;
			}
		}
	}
	return NULL;
}

/*
函数功能：线程池管理,空闲线程过多时减少，小于警戒线时增加
*/
static void *threadPoolManageThread(void *arg)
{
	int    idleNumber = 0;//空闲线程数量
	int    n = 0;
	int    m = 0;
	int    i = 0;
	
	while (1)
	{
		idleNumber = 0;
		
		for (i=0; i<threadPool.size;i++)
		{
			if (threadPool.idle[i] != 0)//计算空闲线程数
			{				//空闲时为1
							//非空闲为0
				idleNumber++;
			}
		}

		printf("threads : %d, idle : %d\n", threadPool.size, idleNumber);

		if (idleNumber < THREAD_ALERT_LINE1)//当空闲数量小于警戒线10个时
		{
			if (threadPool.size < THREAD_MAX_SIZE)//当前线程小于最大线程数1024
			{
				n = (15 - idleNumber); //增加线程数
				m = THREAD_MAX_SIZE - threadPool.size;//剩余的总线程数
				
				addThread((n < m ? n : m));//判定现有线程数是否超过1024
			}
			
		}
		else if (idleNumber > THREAD_ALERT_LINE2)//空闲数大于20，进行线程回收
		{
			subThread(idleNumber - 15);//减少当前的线程数，并维持在第一警戒线
		}
		
		sleep(2);
	}

	return NULL;
}


/*
函数功能：设置空闲线程
形参：    线程标识符，空闲标志
*/
static int setThreadIdle(pthread_t tid, uint8_t flag)
{
	int    i = 0;
	
	for (i=0; i<threadPool.size; i++)
	{
		if (pthread_equal(threadPool.tid[i], tid)) //线程相同,设置为空闲
		{
			threadPool.idle[i] = flag;
			return 0;
		}
	}

	return -1;
}


/*
函数功能：增加线程数
形参：	要增加的线程数
*/
static int addThread(int n)
{
	int    i = 0;
	int    oldSize = 0;
	int    ret = -1;
	
	oldSize = threadPool.size;//老的线程数
	threadPool.size += n;     //增加后的线程总数
	
	for (i=oldSize; i<threadPool.size; i++)//增加的线程数量
	{
		threadPool.idle[i] = 1;	
		ret = pthread_create(&(threadPool.tid[i]), NULL, threadFun, NULL);
		if (ret != 0)
		{
			perror("pthread_create err");
			return -1;
		}
	}

	return 0;
}

/*
函数功能：减少多余的空闲线程数
形参：   要减少的线程数
*/

static int subThread(int n)
{
	int    i = 0;
	int    ret = -1;
	
	i = 0;
	while(i<threadPool.size && n > 0)
	{
		if (threadPool.idle[i] != 0)//取消空闲线程
		{
			pthread_cancel(threadPool.tid[i]);
			threadPool.tid[i] = threadPool.tid[threadPool.size-1];
			threadPool.size--;
			n--;
			continue;
		}
		i++;
	}

	return 0;
}


