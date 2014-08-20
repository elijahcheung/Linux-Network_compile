

#include "Socket.h"

//客户机用
int openSocket(const char *ipstr,
	uint16_t port)
{
	struct sockaddr_in serverAddr;
	int                socketfd;
	int                ret;
	
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
	{

		perror("socket err");
		return -1;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	ret = inet_pton(AF_INET, ipstr, &(serverAddr.sin_addr));
	if (ret < 0)
	{
		perror("inet_pton err");
		close(socketfd);
		return -1;
	}

	ret = connect(socketfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		perror("connect err");
		close(socketfd);
		return -1;
	}

	return socketfd;
}


//主机用
int openListenSocket(uint16_t port)
{
	int                listenSocketfd = -1;
	struct sockaddr_in serverAddr;
	int                ret = -1;
	
	listenSocketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocketfd < 0)
	{

		perror("socket err");
		return -1;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	ret = bind(listenSocketfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		perror("bind err");
		close(listenSocketfd);
		return -1;
	}

	ret = listen(listenSocketfd, 5);
	if (ret < 0)
	{
		perror("listen err");
		close(listenSocketfd);
		return -1;
	}

	return listenSocketfd;
}


ssize_t readN(int fd, void *buf, size_t count)
{
	size_t   left = 0;
	size_t   offset = 0;
	ssize_t  nbytes = 0;

	left = count;
	while (left > 0)
	{
		nbytes = read(fd, buf+offset, left);
		if (nbytes < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			return -1;
		}
		else if (nbytes == 0)
		{
			break;
		}
		else
		{
			left -= nbytes;
			offset += nbytes;
		}
	}

	return offset;
}


ssize_t writeN(int fd, const void *buf, 
	size_t count)
{
	size_t   left = 0;
	size_t   offset = 0;
	ssize_t  nbytes = 0;

	left = count;
	while (left > 0)
	{
		nbytes = write(fd, buf+offset, left);
		if (nbytes < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			return -1;
		}
		else
		{
			left -= nbytes;
			offset += nbytes;
		}
	}

	return offset;
}

ssize_t readLine(int fd, char *buf, 
	size_t count)
{
	size_t   left = 0;   /*bufê￡óàμ?????*/
	size_t   offset = 0;
	size_t   step = 0;
	ssize_t  nbytes = 0;
	int      done = 0;

	left = count;
	step = 1;
	while (left>0 && done==0)
	{
		nbytes = read(fd, buf+offset, step);
		if (nbytes < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			return -1;
		}
		else if (nbytes == 0)
		{
			break;
		}
		else
		{
			if (buf[offset] == '\n')
			{
				done = 1;
			}

			left -= nbytes;
			offset += nbytes;
		}
	}

	return offset;
}




