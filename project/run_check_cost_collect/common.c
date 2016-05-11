#include"common.h"
#include<time.h>

void print_curtline(char *str)
{
	int i,num_print = 100,front = 0,tail = 0;
	int letter_num = strlen(str);
	if(num_print > letter_num)
	{
		front = (num_print - letter_num)/2;
		tail = num_print - front - letter_num;
		for(i=0;i<front;i++)
			printf("=");
		printf("%s",str);
		for(i=0;i<tail;i++)
			printf("=");
	}else 
		printf("%s",str);
	printf("\n");
}
int get_date_and_time(char*in_date,char*in_time)
{

	time_t timep;
	struct tm *p;
	if(-1 == time(&timep))
		perror("get_date_and_time()错误");
	p = localtime(&timep);
	if(!p)
		perror("get_date_and_time()错误");
	sprintf(in_date,"%04d-%02d-%02d",(1900+p->tm_year), (1+p->tm_mon),p->tm_mday);
	sprintf(in_time,"%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec);//2000/10/28 Sat 8:15:38
	//printf("get_date_and_time() %s  %s\n",in_date,in_time);
}


