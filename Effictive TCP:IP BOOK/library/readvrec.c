int readvrec(SOCKET fd,char *bp,size_t len)
{
	u_int32_t reclen;
	int rc;
	
	/* Retrieve length of the record */
	rc = readn(fd,(char*)&reclen,sizeof(u_int32_t));
	if(rc != sizeof(u_int32_t))
	{
		return rc < 0 ? -1 : 0;
	}
	reclen = ntohl(reclen);
	if(reclen > len)
	{
		while(reclen > 0)
		{
			rc = readn(fd,bp,len);
			if(rc != len)
			{
				return rc < 0 ? -1 : 0;
			}
			reclen -= len;
			if(reclen < len)
			{
				len = reclen;
			}
		}
		set_errno(EMSGSIZE);
		return -1;
	}

	/* Retrieve the record itself */
	rc = readn(fd,bp,reclen);
	if(rc != reclen)
	{
		return rc < 0 ? -1 : 0;
	}
	return rc;
}
