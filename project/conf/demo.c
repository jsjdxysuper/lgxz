#include<stdio.h>
#include"inifile.h"
int main()
{
	char ip[60];
	void *stru;
	stru=IniOpenFile("./d5000.ini",0);
	if(stru==NULL)
	{
		printf("read ini file failed!\n");
		return -1;
	}
	if(-1==IniGetString("db","ip",ip,60,stru))
	{
		printf("get string error\n");
	}
	printf("%s",ip);
}
