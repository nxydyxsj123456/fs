#ifndef _FILE_HEADER_
#define _FILE_HEADER_

	#include "INODE.h"

	typedef struct _file{
		INODE * inodeP; //�ļ���INODE����
		int offset;//�ļ�ָ��ĵ�ǰƫ����
	}FILE_FS;

	char getc_FS(FILE_FS * fp);
	void * getContent_FS(FILE_FS * fp);

#endif
