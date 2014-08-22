int readn(SOCKET fd,char *bp,size_t len)
{
	int cnt;
	int rc;

	cnt = len;
	while(cnt > 0)
	{
		rc =recv(fd,bp,cnt,0);
		if(rc < 0)
		{
			if(errno == EINTR)
			{
				continue;
			}
			return -1;
		}
		
		if(rc == 0)
		{
			return len-cnt;
		}
		bp += rc;
		cnt -= rc;
	}
	return len;
}
