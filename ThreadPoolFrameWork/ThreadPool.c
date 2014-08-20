
#include <stdint.h>
#include "ThreadPool.h"

#define THREAD_MAX_SIZE    1024   //����߳�����1024
#define THREAD_INIT_SIZE   15 	//�̳߳�ʼ������15

#define THREAD_ALERT_LINE1 10 //�߳̿�����������
#define THREAD_ALERT_LINE2 20 //�߳̿��������Ƿ����

/*
�̳߳ؽṹ��
*/
typedef struct st_thread_pool
{

	int               listenSocketfd;
	pthread_mutex_t   mutex;
	pthread_t         manageTid;  //�����߳�
	pthread_t         tid[THREAD_MAX_SIZE];//��ǰʹ���߳�����
	uint8_t           idle[THREAD_MAX_SIZE];//�����߳�
	int               size;
} ST_THREAD_POOL;


static ST_THREAD_POOL  threadPool;

static void *threadFun(void *arg);
static void *threadPoolManageThread(void *arg);

static int setThreadIdle(pthread_t tid, uint8_t flag);

static int addThread(int n);
static int subThread(int n);

/*
�������ܣ��̳߳ؿ�ʼ
�βΣ�   �׽���
*/
int startThreadPool(int listenSocketfd)
{
	int     i = 0;
	int     ret = -1;

	threadPool.listenSocketfd = listenSocketfd;
	
	ret = pthread_mutex_init(&(threadPool.mutex),NULL);//��ʼ��������
	if (ret != 0)
	{

		return -1;
	}

	threadPool.size = THREAD_INIT_SIZE;//�߳�������ʼ��15
	for (i=0; i<THREAD_INIT_SIZE; i++)//�����߳�
	{	
		threadPool.idle[i] = 1;	
		ret = pthread_create(&(threadPool.tid[i]), NULL, threadFun, NULL);
		if (ret != 0)
		{
			perror("pthread_create err");
			return -1;
		}

	}

	ret =pthread_create(&(threadPool.manageTid),NULL, threadPoolManageThread, NULL);//�����̴߳���
	if (ret != 0)
	{
		perror("pthread_create err");
		return -1;
	}
	

	return 0;
}


/*
�������ܣ��߳��˳�
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
�������ܣ��߳����Ӻ󣬲���ȡ����
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
		setThreadIdle(pthread_self(),1);//��ȡ��ǰ�̱߳�ʶ��

		pthread_mutex_lock(&(threadPool.mutex));
		clientAddrLen = sizeof(clientAddr);
		
		socketfd=accept(threadPool.listenSocketfd, 
				(struct sockaddr *)&clientAddr,
				&clientAddrLen);
		
		if (socketfd < 0)
		{
			perror("accept err");
			pthread_mutex_unlock(&(threadPool.mutex));
			continue;//����ʧ��ʱ����������
		}
		
		setThreadIdle(pthread_self(), 0);
		//����������׽���������Ϊæ״̬
		
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
�������ܣ��̳߳ع���,�����̹߳���ʱ���٣�С�ھ�����ʱ����
*/
static void *threadPoolManageThread(void *arg)
{
	int    idleNumber = 0;//�����߳�����
	int    n = 0;
	int    m = 0;
	int    i = 0;
	
	while (1)
	{
		idleNumber = 0;
		
		for (i=0; i<threadPool.size;i++)
		{
			if (threadPool.idle[i] != 0)//��������߳���
			{				//����ʱΪ1
							//�ǿ���Ϊ0
				idleNumber++;
			}
		}

		printf("threads : %d, idle : %d\n", threadPool.size, idleNumber);

		if (idleNumber < THREAD_ALERT_LINE1)//����������С�ھ�����10��ʱ
		{
			if (threadPool.size < THREAD_MAX_SIZE)//��ǰ�߳�С������߳���1024
			{
				n = (15 - idleNumber); //�����߳���
				m = THREAD_MAX_SIZE - threadPool.size;//ʣ������߳���
				
				addThread((n < m ? n : m));//�ж������߳����Ƿ񳬹�1024
			}
			
		}
		else if (idleNumber > THREAD_ALERT_LINE2)//����������20�������̻߳���
		{
			subThread(idleNumber - 15);//���ٵ�ǰ���߳�������ά���ڵ�һ������
		}
		
		sleep(2);
	}

	return NULL;
}


/*
�������ܣ����ÿ����߳�
�βΣ�    �̱߳�ʶ�������б�־
*/
static int setThreadIdle(pthread_t tid, uint8_t flag)
{
	int    i = 0;
	
	for (i=0; i<threadPool.size; i++)
	{
		if (pthread_equal(threadPool.tid[i], tid)) //�߳���ͬ,����Ϊ����
		{
			threadPool.idle[i] = flag;
			return 0;
		}
	}

	return -1;
}


/*
�������ܣ������߳���
�βΣ�	Ҫ���ӵ��߳���
*/
static int addThread(int n)
{
	int    i = 0;
	int    oldSize = 0;
	int    ret = -1;
	
	oldSize = threadPool.size;//�ϵ��߳���
	threadPool.size += n;     //���Ӻ���߳�����
	
	for (i=oldSize; i<threadPool.size; i++)//���ӵ��߳�����
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
�������ܣ����ٶ���Ŀ����߳���
�βΣ�   Ҫ���ٵ��߳���
*/

static int subThread(int n)
{
	int    i = 0;
	int    ret = -1;
	
	i = 0;
	while(i<threadPool.size && n > 0)
	{
		if (threadPool.idle[i] != 0)//ȡ�������߳�
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


