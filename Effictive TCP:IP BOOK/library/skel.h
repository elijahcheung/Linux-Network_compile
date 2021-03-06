#ifndef _SKEL_H
#define _SKEL_H

#define INIT() (program_name = strrchr(argv[0],'/')) ? program_name++ :\
				(program_name = argv[0])

#define EXIT(s) exit(s)
#define CLOSE(s) if(close(s)) error(1,errno,"close failed")
#define set_errno(e) errno = (e)
#define isvalidsock(s) ((s) >= 0)

typedef int SOCKET;

#endif
