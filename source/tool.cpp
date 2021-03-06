#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "file.h"
#include "tool.h"
#include "superBlock.h"

extern SUPER_BLOCK * superBlockPointer;
extern User currentUser;
extern FILE * dataFp ;

char ** parse(char * s)
{
	s = ltrim(s, ' ');
	char ** p = (char**)Malloc(sizeof(char *)* PARSE_TOKEN_LENGTH);
	char * _tmp = NULL;
	int i = 0;
	_tmp = strtok(s, " ");
	while (TRUE){
		if (_tmp == NULL){
			break;
		}
		else{
			_tmp = ltrim(_tmp, ' ');
			p[i] = _tmp;
		}
		_tmp = strtok(NULL, " ");
		i++;
	}
	return p;
}


void * Malloc(unsigned int Size_t)
{
	void * _p = malloc(Size_t);
	if (_p == NULL){
		printf("内存申请失败！\r\n");
		getchar();
		exit(-3);
	}
	memset(_p, 0, Size_t);
	return _p;
}

int Fwrite(void * mem, size_t size, size_t count, FILE * fp)
{
	//为了调试立马看结果……不知道FILE对象的缓冲在哪
	int retCount = fwrite(mem, size, count, fp);
	fflush(fp);
	return retCount;
}

char * ltrim(char * s, char c)
{
	if (NULL == s){
		return NULL;
	}
	char * p = s;
	while (*p == c){
		p++;
	}
	return p;
}


BOOL login()
{
	char username[16];
	char password[16];
	printf("input Username:\r\n");
	while (scanf("%s", username) < 1){
		printf("input Username;\r\n");
		continue;
	}
	fflush(stdin);
	printf("input Password;\r\n"); //这个地方需要让屏幕不进行回显
	while (scanf("%s", password) < 1){
		printf("input Password:\r\n");
		continue;
	}
	return _login(username, password);
}

BOOL _login(char * username, char * password)
{
	FILE_FS * fileFsP = openFile("/user");
	if(fileFsP == NULL){
		printf("Can not open user file!\r\n");
		//printf("无法打开用户文件，登陆失败，或者删除data.txt文件重新运行模拟程序！\r\n");
		return FALSE;
	}
	User * userP = getUser(fileFsP);
	if(userP != NULL && strcmp(userP->username, username) == 0 && strcmp(userP->passwd, password) == 0){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

INODE * getInode(char * path) //等价于书上的NameI()
{
	if(NULL == path){
		return NULL;
	}
	if(path[0] != '/'){
		return NULL;
	}
	if(strcmp(path, "/") == 0){
		//return getINODE(superBlockPointer->inode->inodeNumber);
		return superBlockPointer->inode;
	}
	INODE * inodeP = superBlockPointer->inode;
	char * s = strtok(path+1, "/");
	int inodeNumber;
	FILE_FS * fileFsP = NULL;
	BOOL flag = TRUE;
	while(TRUE){
		if(s == NULL){
			break;
		}
		else{
			fileFsP = createFILE_FS(inodeP);
			inodeNumber = getFileInodeInFolder(fileFsP, s);
			if(inodeNumber){
				INODE * tmp = inodeP;
				inodeP = getINODE(inodeNumber);
				if(flag){
					flag = FALSE;
				}
				else{
					freeInode(tmp);
					free(fileFsP->mem);
					free(fileFsP);
				}
			}
			else{
				if(flag){
					flag = FALSE;
				}
				else{
					free(fileFsP->mem);
					free(fileFsP);
				}
				return NULL;
			}
		}
		s = strtok(NULL, "/");
	}
	return inodeP;
}

void writeBootSector()
{
	char spaces[] = "boot sector ";
	int i = 0;
	while(i<(512/sizeof(spaces))){
		Fwrite(spaces,sizeof(spaces), 1,dataFp);
		i++;
	}
	i = 0;
	while(i< (512%sizeof(spaces))){
		Fwrite(" ",1, 1,dataFp);
		i++;
	}
}

int countMem(void * mem, int length)
{
	char * str = (char*)mem;
	if(NULL == mem){
		return 0;
	}
	int count = 0;
	unsigned char ch;
	int i = 0;
	while(i < length){
		ch = str[i];
		count += countChar(ch);
		i++;
	}
	return count;
}

int countChar(unsigned char ch)
{
	int count = 0;
	while(ch % 2){
		ch = ch >> 1;
		count ++;
	}
	return count;
}

int findZero(void * mem, int length) //查找并设置bitmap
{
	char * str = (char*)mem;
	if(NULL == mem){
		return 0;
	}
	int pos = 0;
	unsigned char ch;
	int i = 0;
	while(i < length){
		ch = str[i];
		if(ch == (unsigned char)255){
			pos += 8;
		}
		else{
			int tmp = countChar(ch);
			unsigned char a = 1 << tmp;
			str[i] = ch | a;
			return pos + tmp;
		}
		i++;
	}
	return -1;
}

int setZero(void * mem, unsigned int length, unsigned int pos)
{
	if(pos > length * 8){
		return -1;
	}
	char * str = (char *) mem;
	int posChar = pos / 8;
	unsigned int posBit = pos % 8;
	char c = str[posChar];
	switch(posBit){
	case 0:
		c = c & 0xFE;
		break;
	case 1:
		c = c & 0xFD;
		break;
	case 2:
		c = c & 0xFB;
		break;
	case 3:
		c = c & 0xF7;
		break;
	case 4:
		c = c & 0xEF;
		break;
	case 5:
		c = c & 0xDF;
		break;
	case 6:
		c = c & 0xBF;
		break;
	case 7:
		c = c & 0x7F;
		break;
	}
	str[posChar] = c;	
	return 0;
}

BOOL hasCreateFileAuthority(INODE * inodeP, User * userP)
{
	if(userP->UID == 0){
		//ROOT 则返回TRUE
		return TRUE;
	}
	if(userP->UID == inodeP->UID){
		return TRUE;
	}
	if(userP->GID == inodeP->GID){
		if(inodeP->authority & _GROUP_WRITE_DEFINE_)
			return TRUE;
		else
			return FALSE;
	}
	if(inodeP->authority & _OTHER_WRITE_DEFINE_){
		return TRUE;
	}
	return FALSE;
}

BOOL hasReadAuthority(INODE * inodeP, User * userP)
{
	if(userP->UID == 0){
		//ROOT 则返回TRUE
		return TRUE;
	}
	if(userP->UID == inodeP->UID){
		return TRUE;
	}
	if(userP->GID == inodeP->GID){
		if(inodeP->authority & _GROUP_READ_DEFINE_)
			return TRUE;
	}
	if(inodeP->authority & _OTHER_READ_DEFINE_){
		return TRUE;
	}
	return FALSE;
}

BOOL hasWriteAuthority(INODE * inodeP, User * userP)
{
	if(userP->UID == 0){
		//ROOT 则返回TRUE
		return TRUE;
	}
	if(userP->UID == inodeP->UID){
		return TRUE;
	}
	if(userP->GID == inodeP->GID){
		if(inodeP->authority & _GROUP_WRITE_DEFINE_){
			return TRUE;
		}
	}
	if(inodeP->authority & _OTHER_WRITE_DEFINE_){
		return TRUE;
	}
	return FALSE;
}

BOOL hasGetIntoAuthority(INODE * inodeP, User * userP)
{
	if(userP->UID == 0){
		//ROOT 则返回TRUE
		return TRUE;
	}
	if(userP->UID == inodeP->UID){
		return TRUE;
	}
	if(userP->GID == inodeP->GID){
		if(inodeP->authority & _GROUP_EXEC_DEFINE_){
			return TRUE;
		}
	}
	if(inodeP->authority & _OTHER_EXEC_DEFINE_){
		return TRUE;
	}
	return FALSE;
}

void writeNull(unsigned int count, FILE * fp)
{
	char c = 0;
	int i = 0;
	while(i < count){
		fwrite(&c, 1, 1, fp);
		i++;
	}
}

void writeChar(char * s, int count, FILE * fp) //把某个字符写到文件当前位置count次
{
	int i = 0;
	while(i < count){
		fwrite(s, strlen(s), 1, fp);
	}
}

void * pathCat(char * path, char * path2)
{
	if(path == NULL || path2 == NULL){
		return NULL;
	}
	if(strcmp(path, "/") == 0){
		strcat(path, path2);
		return path;
	}
	else{
		int length = strlen(path);
		if(path[length-1] == '/'){
			strcat(path, path2);
		}
		else{
			strcat(path, "/");
			strcat(path, path2);
		}
		return path;
	}
}

void setUmask(User * userP, char * s)
{
	if(NULL == s){
		printf("param is not valid!\r\n");
		return;
	}
	int len = strlen(s);
	if(len != 3){
		printf("mask length is not right!\r\n");
		return;
	}
	if(!isDigit(s)){
		printf("mask is not right!\r\n");
		return;
	}
	int mask = 0;
	char ch = s[2];
	int value = ch - '0';
	if(value < 0 || value > 7){
		return;
	}
	mask = value;
	ch = s[1];
	value = ch - '0';
	if(value < 0 || value > 7){
		return;
	}
	mask += value * 8;
	ch = s[0];
	value = ch - '0';
	if(value < 0 || value > 7){
		return;
	}
	mask += value * 8 * 8;
	userP->umask = mask;
}

BOOL isDigit(char * s)
{
	if(NULL == s){
		return FALSE;
	}
	int len = strlen(s);
	int i = 0;
	while(i < len){
		if(!isdigit(s[i])){
			return FALSE;
		}
		i++;
	}
}
