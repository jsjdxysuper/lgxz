#include<stdio.h>
void fun(int **a)
{
	*a=(int*)malloc(4);
	**a=5;
}
int main()
{
	char a[20]="1234566";
	strcpy(a,"12345");
	printf("%s\n",a);
}
