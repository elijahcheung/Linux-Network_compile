#include "etcp.h"

SOCKET tcp_server(char *hname,char* sname)
{
	struct sockaddr_in local;
	SOCKET s;
	const int on = 1;

	set_address(hname,sname,&local,"tcp");
	s = socket(AF_INET,SOCK_STREAM,0);
	if(!isvalidsock(s))
	{
		error(1,errno,"socket call failed");
	}

	if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on)))
	{
		error(1,errno,"setsockopt failed");
	}//end of if

	if(bind(s,(struct sockaddr*)&local,sizeof(local)))
	{
		error(1,errno,"bind failed");
	}//end of if

	if(listen(s,10))
	{
		error(1,errno,"listen failed");
	}//end of if
	
	return s;
}
