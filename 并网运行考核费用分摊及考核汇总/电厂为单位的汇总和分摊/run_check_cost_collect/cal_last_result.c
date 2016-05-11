#include<stdio.h>
#include"api.h"
#include"log.h"
#include"dm_operater.h"
#define COST_PER_MARK 1000	//���ַ�ֵ��Ӧ����ҵ���Ŀ
#define  PERROR_CLOSEDATABASE_EXIT_IF(dec,ret,str) if((dec)==DM_FALSE)\
        {fprintf(stderr,"error in: %s,file:%s,line:%d\n",str,__FILE__,__LINE__);close_database();exit(ret);}
#define  PERROR_CLOSEDATABASE_EXIT(ret,str)\
		{fprintf(stderr,"error in: %s,file:%s,line:%d\n",str,__FILE__,__LINE__);close_database();exit(ret);}

//�����п��˷��÷�̯��run_check_cost_shareһ�µ����ݽṹ
typedef struct
{
	char year_month[10];     //����
	char plant_name[100];	 //�糧����
	double month_electricity;//�¶���������
	double check_cost;		 //���˷���
	double share_cost;		 //��̯����
	double account_cost;	 //�������
}cost_share_stru;

//=======================================================================================
/*
����:�����ݿ������Ϣ��unit_info��ȡ�糧���֣���̬����ռ䣬����ʼ��cost_share_stru�ṹ
����:plant_num���ڴ�����̬����cost_share_stru�ṹ�ĵ�Ԫ����
����ֵ:��������ռ�ָ��
*/
//======================================================================================
cost_share_stru * get_unit_info(char *month,long long* plant_num)
{
	char sql_string[256];
	char plant_name[256];
	long plant_name_size;
	long long i,row_num;
	cost_share_stru *run_check_cost_share;
	int flag;//�ж�dm_fetch_scroll()�Ƿ�ɹ�
	sprintf(sql_string,"%s","select distinct sstation from plan.plan.unit_info");

	if (exec_sql(sql_string) != 0)
	{
		fprintf(stderr,"get_unit_info()��exec_sql()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
		close_database();
		return(NULL);
	}
	if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
	{
		fprintf(stderr,"get_unit_info()��dm_bind_column()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
		close_database();
		return(NULL);
	}

	if(DM_FALSE==dm_describe_row_num(hsmt,plant_num))
	{
		fprintf(stderr,"get_unit_info()��dm_bind_column()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
		close_database();
		return(NULL);
	}

	run_check_cost_share=(cost_share_stru*)malloc((*plant_num)*sizeof(cost_share_stru));
	if(run_check_cost_share==NULL)
	{
		fprintf(stderr,"get_unit_info()��malloc()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
		close_database();
		return(NULL);
	}
	else
	{
		for(i=0;i<*plant_num;i++)
		{
			run_check_cost_share[i].account_cost=0;
			run_check_cost_share[i].check_cost=0;
			run_check_cost_share[i].month_electricity=0;
			run_check_cost_share[i].share_cost=0;
			strcpy(run_check_cost_share[i].year_month,month);
		}
	}

	for(i=0;i<*plant_num;i++)
	{
		flag=dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
		{
			fprintf(stderr,"get_unit_info()��dm_fetch_scroll()������,��%d��ѭ��,��%d,�ļ�%s\n",i,__LINE__,__FILE__);
			close_database();
			return(NULL);
		}
		else if(DM_NO_DATA_FOUND==flag)
		{
			fprintf(stderr,"select distinct sname from plan.plan.month_electricityִ��ʱ,���ݱ���û������,get_unit_info()��dm_fetch_scroll()��,��%d��ѭ��,��%d,�ļ�%s\n",i,__LINE__,__FILE__);
			close_database();
			return(NULL);
		}
		strcpy(run_check_cost_share[i].plant_name,plant_name);
	}
	
	printf("\nget_unit_info():\n��Ԫռ�ÿռ�:%d,һ����Ԫ��:%d*%d\n",sizeof(cost_share_stru),*plant_num,sizeof(cost_share_stru));
	for(i=0;i<*plant_num;i++)
	{
		printf("%s\n",run_check_cost_share[i].plant_name);
	}
	printf("\nget_unit_info()����\n");
	return run_check_cost_share;
}

//=======================================================================================
/*
����:�����ݿ�ȡ��Ӧ�糧�Ŀ��˽��
����:month����Ҫ������·ݵ����߿��˽��
	:cost_share_arrayָ�� ���п��˷��÷�̯�� �ṹ��ָ��
	:plant_num��ṹ�е�Ԫ��Ŀ
����ֵ:����-1ִ��ʧ�ܣ�0ִ�гɹ�
*/
//======================================================================================
int get_cost_result(char *month,cost_share_stru **cost_share_array,long long* plant_num)
{
	
	long long i,j;
	long long row_num;
	char sql_string[256];
	char plant_name[256];
	long plant_name_size;
	char check_cost[20];
	long check_cost_size;
	int flag;

	//===================================================================================	
	/*ȡ�üƻ����˽��
	*/
	//================================================================================


	sprintf(sql_string,"select sstation,sum(result_num) from\
		plan.plan.unit_info,plan.plan.plan_check_result\
		where plan.plan.unit_info.sname=plan.plan.plan_check_result.sname\
		and ('%s-00'< sdate and sdate < '%s-32')\
		group by plan.plan.unit_info.sstation",month,month);

	//PERROR_CLOSEDATABASE_RETURN_IF(0,-1,"�����õ�")
	//PERROR_CLOSEDATABASE_RETURN_IF(dec,ret,str)
	//PERROR_CLOSEDATABASE_RETURN(ret,str)
	//PERROR_CLOSEDATABASE_RETURN(-1,"�����õ�")
	//if(1)
	//	PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()��exec_sql()")
	if (exec_sql(sql_string) != 0)	
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()��exec_sql()")
	
	if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
		plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()��dm_bind_column()")

	if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_DOUBLE,TYPE_DEFAULT,
		check_cost,sizeof(check_cost),&check_cost_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()��dm_bind_column()")

	if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()��dm_describe_row_num()")

	for(i=0;i<row_num;i++)
	{
		flag=dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()��dm_fetch_scroll()")

		else if(DM_NO_DATA_FOUND==flag)
		{
			/*fprintf(stderr,"%sִ��ʱ,���ݱ���û������,get_cost_result()��dm_fetch_scroll()��,��%d��ѭ��,��%d,�ļ�%s\n",
				sql_string,i,__LINE__,__FILE__);*/
			PERROR_CLOSEDATABASE_EXIT(-1,"����?���ݱ���û������,get_cost_result()��dm_fetch_scroll()")
		}
		for(j=0;j<*plant_num;j++)
		{
			//printf("%s,%s,i=%d,j=%d\n",plant_name,check_cost,i,j);	
			//printf("plant_name=%s,cost_share_array[j].plant_name=%s,%d,%d\n",plant_name,cost_share_array[j].plant_name,strlen(plant_name),strlen(cost_share_array[j].plant_name));
			if(0==strcmp(plant_name,(*cost_share_array)[j].plant_name))
			{	
				//printf("\n���%f\n",atof(check_cost));
							
				(*cost_share_array)[j].check_cost+=(atof(check_cost)*COST_PER_MARK);
				//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
				break;
			}
		}
		
	}
	printf("ֻ�мƻ����߿���ʱ:��������Ŀ%d\n",plant_num);
		printf("����������	   ,���̿��˷���\n");
		for(i=0;i<*plant_num;i++)
		{
			printf("%s, 	%f\n",(*cost_share_array)[i].plant_name,(*cost_share_array)[i].check_cost);
		}

//===================================================================================	
/*ȡ�÷�ͣ���˽��
*/
//================================================================================
	/*select sstation,sum(result_num) from
		plan.plan.unit_info,plan.plan.unplan_shutdown_result
		where plan.plan.unit_info.sname=plan.plan.unplan_shutdown_result.sname
		and ('2013-05-00'< sdate and sdate < '2013-05-32')
		group by plan.plan.unit_info.sstation*/
	sprintf(sql_string,"select sstation,sum(result_num) from\
		plan.plan.unit_info,plan.plan.unplan_shutdown_result\
		where plan.plan.unit_info.sname=plan.plan.unplan_shutdown_result.sname\
		and ('%s-00'< sdate and sdate < '%s-32')\
		group by plan.plan.unit_info.sstation",month,month);


	if (exec_sql(sql_string) != 0)
	{
		fprintf(stderr,"get_unplan_shutdown_result()��exec_sql()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}
	if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
		plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
	{
		fprintf(stderr,"get_unplan_shutdown_result()��dm_bind_column()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}		
	if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_DOUBLE,TYPE_DEFAULT,
		check_cost,sizeof(check_cost),&check_cost_size,DM_DESC_PTR_LEN_DEFAULT))
	{
		fprintf(stderr,"get_unplan_shutdown_result()��dm_bind_column()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}	

	if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
	{
		fprintf(stderr,"get_unplan_shutdown_result()��dm_describe_row_num()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}

	for(i=0;i<row_num;i++)
	{
		flag=dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
		{
			fprintf(stderr,"get_unplan_shutdown_result()��dm_fetch_scroll()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
			close_database();
			return(-1);
		}
		else if(DM_NO_DATA_FOUND==flag)
		{
			fprintf(stderr,"%sִ��ʱ,���ݱ���û������,get_unplan_shutdown_result()��dm_fetch_scroll()��,��%d��ѭ��,��%d,�ļ�%s\n",
				sql_string,i,__LINE__,__FILE__);
			close_database();
			return(-1);
		}
		for(j=0;j<*plant_num;j++)
		{
			//printf("%s,%s,i=%d,j=%d\n",plant_name,check_cost,i,j);	
			//printf("plant_name=%s,cost_share_array[j].plant_name=%s,%d,%d\n",plant_name,cost_share_array[j].plant_name,strlen(plant_name),strlen(cost_share_array[j].plant_name));
			if(0==strcmp(plant_name,(*cost_share_array)[j].plant_name))
			{	
				//printf("\n���%f\n",atof(check_cost));
							
				(*cost_share_array)[j].check_cost+=(atof(check_cost)*COST_PER_MARK);
				//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
				break;
			}
		}
		
	}
	printf("�����ͣ���˽����:��������Ŀ%d\n",plant_num);
	printf("����������     ,���̿��˷���\n");
	for(i=0;i<*plant_num;i++)
	{
		printf("%s,     %f\n",(*cost_share_array)[i].plant_name,(*cost_share_array)[i].check_cost);
	}


//=========================================================================================
//������й������֣���дcost_share_stru�ṹ����
//select sname ,sum(svalue)from plan.plan.run_manage_check where sdate<'2013-05-32'and sdate>'2013-05-00' group by sname
//=========================================================================================

	sprintf(sql_string,"select sname ,sum(svalue)from\
	plan.plan.run_manage_check where sdate<'%s-32'and\
	sdate>'%s-00' group by sname",month,month);

	if (exec_sql(sql_string) != 0)
	{
		fprintf(stderr,"get_unplan_shutdown_result()��exec_sql()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}
	if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
		plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
	{
		fprintf(stderr,"get_unplan_shutdown_result()��dm_bind_column()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}		
	if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_DOUBLE,TYPE_DEFAULT,
		check_cost,sizeof(check_cost),&check_cost_size,DM_DESC_PTR_LEN_DEFAULT))
	{
		fprintf(stderr,"get_unplan_shutdown_result()��dm_bind_column()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}	

	if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
	{
		fprintf(stderr,"get_unplan_shutdown_result()��dm_describe_row_num()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}

	for(i=0;i<row_num;i++)
	{
		flag=dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
		{
			fprintf(stderr,"get_unplan_shutdown_result()��dm_fetch_scroll()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
			close_database();
			return(-1);
		}
		else if(DM_NO_DATA_FOUND==flag)
		{
			fprintf(stderr,"%sִ��ʱ,���ݱ���û������,get_unplan_shutdown_result()��dm_fetch_scroll()��,��%d��ѭ��,��%d,�ļ�%s\n",
				sql_string,i,__LINE__,__FILE__);
			close_database();
			return(-1);
		}
		for(j=0;j<*plant_num;j++)
		{
			//printf("%s,%s,i=%d,j=%d\n",plant_name,check_cost,i,j);	
			//printf("plant_name=%s,cost_share_array[j].plant_name=%s,%d,%d\n",plant_name,cost_share_array[j].plant_name,strlen(plant_name),strlen(cost_share_array[j].plant_name));
			if(0==strcmp(plant_name,(*cost_share_array)[j].plant_name))
			{	
				//printf("\n���%f\n",atof(check_cost));
							
				(*cost_share_array)[j].check_cost+=(atof(check_cost)*COST_PER_MARK);
				//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
				break;
			}
		}
		
	}
	printf("�������п��˽����:��������Ŀ%d\n",plant_num);
	printf("����������     ,���̿��˷���\n");
	for(i=0;i<*plant_num;i++)
	{
		printf("%s,     %f\n",(*cost_share_array)[i].plant_name,(*cost_share_array)[i].check_cost);
	}
	
	return 0;
}
//=================================================================================================
int get_month_electricity(char *month,cost_share_stru **cost_share_array,long long* plant_num)
{
	/*select  sname ,max_power from plan.plan.month_electricity where sdate='2013-05'*/
	long long i,j;
	long long row_num;
	char sql_string[256];
	char plant_name[256];
	long plant_name_size;
	char month_power[20];
	long month_power_size;
	int flag;
	
	


	
	sprintf(sql_string,"select  sname ,max_power from plan.plan.month_electricity where sdate='%s'",month);
	printf("sql_string=%s\n",sql_string);
	
		if (exec_sql(sql_string) != 0)
		{
			fprintf(stderr,"get_month_electricity()��exec_sql()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
			close_database();
			return(-1);
		}
		if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
			plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
		{
			fprintf(stderr,"get_month_electricity()��dm_bind_column()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
			close_database();
			return(-1);
		}		
		if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_DOUBLE,TYPE_DEFAULT,
			month_power,sizeof(month_power),&month_power_size,DM_DESC_PTR_LEN_DEFAULT))
		{
			fprintf(stderr,"get_month_electricity()��dm_bind_column()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
			close_database();
			return(-1);
		}	
	
		if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
		{
			fprintf(stderr,"get_month_electricity()��dm_describe_row_num()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
			close_database();
			return(-1);
		}
	
		for(i=0;i<row_num;i++)
		{
			flag=dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
			if(DM_FALSE==flag)
			{
				fprintf(stderr,"get_month_electricity()��dm_fetch_scroll()������,��%d,�ļ�%s\n",__LINE__,__FILE__);
				close_database();
				return(-1);
			}
			else if(DM_NO_DATA_FOUND==flag)
			{
				fprintf(stderr,"%sִ��ʱ,���ݱ���û������,get_month_electricity()��dm_fetch_scroll()��,��%d��ѭ��,��%d,�ļ�%s\n",
					sql_string,i,__LINE__,__FILE__);
				close_database();
				return(-1);
			}
			printf("month_power=%f\n",atof(month_power));
			for(j=0;j<*plant_num;j++)
			{
				//printf("%s,%s,i=%d,j=%d\n",plant_name,check_cost,i,j);	
				//printf("plant_name=%s,cost_share_array[j].plant_name=%s,%d,%d\n",plant_name,cost_share_array[j].plant_name,strlen(plant_name),strlen(cost_share_array[j].plant_name));
				if(0==strcmp(plant_name,(*cost_share_array)[j].plant_name))
				{	
					//printf("\n���%f\n",atof(check_cost));
								
					(*cost_share_array)[j].month_electricity+=atof(month_power);
					
					//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
					break;
				}
			}
			
		}
		printf("�����¶���������:��������Ŀ%d\n",*plant_num);
		printf("����������	   ,���̿��˷���\n");
		for(i=0;i<*plant_num;i++)
		{
			printf("%s, 	%f,     %f\n",(*cost_share_array)[i].plant_name,(*cost_share_array)[i].check_cost,(*cost_share_array)[i].month_electricity);
		}
}
//======================================================================================
int write_share_stru(cost_share_stru **cost_share_array,long long* plant_num)
{
	double sum_month_electricity=0,sum_check_cost=0,sum_accout_cost=0;
	long long i;
	//���㿼���ֺܷ��¶�����������
	for(i=0;i<*plant_num;i++)
	{
		sum_month_electricity+=(*cost_share_array)[i].month_electricity;
		sum_check_cost+=(*cost_share_array)[i].check_cost;
	}
	//���ṹ����д��̯���úͽ������
	for(i=0;i<*plant_num;i++)
	{
		(*cost_share_array)[i].share_cost=(*cost_share_array)[i].month_electricity/sum_month_electricity*sum_check_cost;
		(*cost_share_array)[i].account_cost=(*cost_share_array)[i].share_cost-(*cost_share_array)[i].check_cost;
	}
	for(i=0;i<*plant_num;i++)
	{
		sum_accout_cost+=(*cost_share_array)[i].account_cost;
	}
	if(abs(sum_accout_cost)>0.001)
		printf("\n\n\n��Ϊ�������!%f\n\n\n",sum_accout_cost);


	printf("�����:��������Ŀ%d\n",*plant_num);
	printf("��  ��,�� �� �� �� ��,�¶������������� �� �� ��,�� ̯ �� �ã��� �� �� ��\n");
	for(i=0;i<*plant_num;i++)
	{
		printf("%s, 	%s,   %f     %f    %f    %f\n",(*cost_share_array)[i].year_month,(*cost_share_array)[i].plant_name,(*cost_share_array)[i].month_electricity,(*cost_share_array)[i].check_cost,(*cost_share_array)[i].share_cost,(*cost_share_array)[i].account_cost);
	}
	
}
int main(int argc,char **argv)
{
	printf("\n\n\n");
	long long i;
	if(init_database()!=0)
		return(-1);
	//get_cost_result(argv[1]);
	cost_share_stru *cost_share_array;
	long long plant_num;



	
	cost_share_array=get_unit_info(argv[1],&plant_num);
	
	if(-1==get_cost_result(argv[1],&cost_share_array,&plant_num))
		{printf("ִ��get_unplan_shutdown_result()ʧ��\n");return -1;}


	if(-1==get_month_electricity(argv[1],&cost_share_array,&plant_num))
		{printf("ִ��get_month_electricity()ʧ��\n");return -1;}
	
	if(-1==write_share_stru(&cost_share_array,&plant_num))
		{printf("ִ��write_share_stru()ʧ��\n");return -1;}




	
	free(cost_share_array);
	close_database();
	printf("\n\n");
}


