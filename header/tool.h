#pragma once

#ifndef _TOOL_HEADER_

#define _TOOL_HEADER_

#define BOOL int
#define TRUE 1
#define FALSE 0

#define PARSE_TOKEN_LENGTH 5

typedef struct _user{
	char * username;
	char * passwd;
	int UID;
	int GID;
}User;
#endif

char ** parse(char * s);
void * Malloc(unsigned int Size_t);
char * ltrim(char * s, char c);
void login();
BOOL _login(char * username, char * password);