#include <stdio.h>
#include <string.h>

#include "tool.h"

#define PWD_LENGTH 1024

char currentPwd[PWD_LENGTH];
User currentUser;

void run()
{
	char command[16];
	char ** cmds = NULL;
	int n = 0;

	while (TRUE)
	{
		n = scanf("%[^\n]", command);
		if (n < 1){
			continue;
		}
		cmds = parse(command);
		if (strcmp(cmds[0], "cd") == 0){
			//����Ŀ¼
		}
		else if (strcmp(cmds[0], "touch") == 0){
			//�����ļ�
		}
		else if (strcmp(cmds[0], "rm") == 0){
			//ɾ���ļ�
		}
		else if (strcmp(cmds[0], "mkdir") == 0){
			//�����ļ���
		}
		else if (strcmp(cmds[0], "rmdir") == 0){

		}
		else if (strcmp(cmds[0], "ls") == 0){
			//���е�ǰĿ¼���ļ�����
		}
		else if (strcmp(cmds[0], "write") == 0){
			//д�ļ�����
		}
		else if (strcmp(cmds[0], "read") == 0){
			//���ļ�����

		}
		else if (strcmp(cmds[0], "umask") == 0){
			//�ļ���Ŀ¼Ȩ������
		}
		else{
			continue;
		}
	}
}

void main()
{
	//���е�½���
	login();
	run();
}
