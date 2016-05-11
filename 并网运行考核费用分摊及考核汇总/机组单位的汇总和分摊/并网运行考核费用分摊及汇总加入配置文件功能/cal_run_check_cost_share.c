/*
*cal_last_result.c
*
*����:�������п��˷��÷�̯
*
*
**/

#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include"api.h"
#include"log.h"
#include"dm_operater.h"
#include"cal_run_check_cost_share.h"


//����ִ����Ϣ
act_info_stru act_info_to_write;
#define COLLECT_NUM		17

char cost_collect_order[][22]=
{
	{"AGC����"},//0
	{"��ȫ����"},//1
	{"�糧��ѹ������豸"},//2
	{"����ҵ�����"},//3
	{"�������"},//4
	{"�Ǽƻ�ͣ������"},//5
	{"���Ԥ�⿼��"},//6
	{"����������"},//7
	{"�������"},//8
	{"�̵籣��"},//9
	{"���ź�PSSװ�ù���"},//10
	{"����ΥԼ����"},//11
	{"ˮ�����"},//12
	{"ͨ��"},//13
	{"�޹����ڷ���"},//14
	{"һ�ε�Ƶ����"},//15
	{"�Զ���"}//16
};
typedef struct
{
	char type[20];
	char month[20];
}para_stru;
para_stru para;
//=======================================================================================
/*
*����:�����ݿ������Ϣ��unit_info��ȡ�糧���֣���̬����ռ䣬����ʼ��cost_share_stru�ṹ
*����:plant_num���ڴ�����̬����cost_share_stru�ṹ�ĵ�Ԫ����
*����ֵ:��������ռ�ָ��
*/
//======================================================================================
int get_unit_info(cost_share_stru **run_check_cost_share, cost_collect_stru **cost_collect17,
char *month, long long* plant_num)
{
	char sql_string[256];
	char plant_name[256];
	long plant_name_size;
	long long i, j, row_num;
	int  flag;           //�ж�dm_fetch_scroll()�Ƿ�ɹ�

	printf("\n\n\nhere\n\n\n");
	sprintf(sql_string, "%s", "select distinct sstation from plan.plan.unit_info");
	if (exec_sql(sql_string) != 0)
		PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()��exec_sql()")
		
	if (DM_FALSE == dm_bind_column(hsmt, 1, DM_DATA_CHAR, TYPE_DEFAULT, 
		plant_name, sizeof(plant_name), &plant_name_size, DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_unit_info()��dm_bind_column()")


	if(DM_FALSE == dm_describe_row_num(hsmt, plant_num) )
		PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()��dm_describe_row_num()")

	*run_check_cost_share = (cost_share_stru*)malloc((*plant_num)*sizeof(cost_share_stru) );
	if(run_check_cost_share==NULL)
		PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()��malloc()")
	else
	{
		for(i=0; i<*plant_num; i++)
		{
			(*run_check_cost_share)[i].account_cost = 0;
			(*run_check_cost_share)[i].check_cost = 0;
			(*run_check_cost_share)[i].month_electricity = 0;
			(*run_check_cost_share)[i].share_cost = 0;
			strcpy( (*run_check_cost_share)[i].year_month, month);
		}
	}




	*cost_collect17 = (cost_collect_stru*)malloc( (*plant_num)*sizeof(cost_collect_stru) );
	if(cost_collect17==NULL)
		PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()��malloc()")
	else
	{
		for(i=0; i<*plant_num; i++)
		{
			for(j= 0;j<COLLECT_NUM;j++)
			(*cost_collect17)[i].value[j] = 0;
		}
	}


	
	for(i=0;i<*plant_num;i++)
	{
		flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE == flag)
			PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()��dm_fetch_scroll()")
		else if(DM_NO_DATA_FOUND == flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"select distinct sname from "
				"plan.plan.month_electricityִ��ʱ,���ݱ���û������,get_unit_info()��dm_fetch_scroll()")
				
		strcpy((*run_check_cost_share)[i].plant_name, plant_name);
		strcpy((*cost_collect17)[i].plant_name, plant_name);
	}
	
	printf("\nget_unit_info():\n��Ԫռ�ÿռ�:%d,һ����Ԫ��:%d*%d\n",sizeof(cost_share_stru),*plant_num,sizeof(cost_share_stru));
	for(i=0;i<*plant_num;i++)
	{
		printf("%s\n",(*run_check_cost_share)[i].plant_name);
		printf("%s\n",(*cost_collect17)[i].plant_name);
	}
	printf("\nget_unit_info()����\n");
	return 0;
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
int get_cost_result(char *month,cost_share_stru **cost_share_array, cost_collect_stru **cost_collect17, long long* plant_num)
{
	
	long long i,j,k;
	long long row_num;
	char sql_string[256];
	char plant_name[256];
	long plant_name_size;
	char check_cost[20];
	long check_cost_size;
	int  flag;
	char field[22];
	long field_size;

	//===================================================================================	
	/*ȡ�üƻ����˽��
	*/
	//================================================================================


	sprintf(sql_string,"select sstation,sum(result_num) from\
		plan.plan.unit_info,plan.plan.plan_check_result\
		where plan.plan.unit_info.sname=plan.plan.plan_check_result.sname\
		and ('%s-00'< sdate and sdate < '%s-32')\
		group by plan.plan.unit_info.sstation",month,month);

	
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
		flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()��dm_fetch_scroll()")

		else if(DM_NO_DATA_FOUND==flag)
		{
			/*fprintf(stderr,"%sִ��ʱ,���ݱ���û������,get_cost_result()��dm_fetch_scroll()��,��%d��ѭ��,��%d,�ļ�%s\n",
				sql_string,i,__LINE__,__FILE__);*/
			PERROR_CLOSEDATABASE_EXIT(-1,"ִ��sqlʱ,���ݱ���û������,get_cost_result()��dm_fetch_scroll()")
		}
		for(j=0;j<*plant_num;j++)
		{
			//printf("%s,%s,i=%d,j=%d\n",plant_name,check_cost,i,j);	
			//printf("plant_name=%s,cost_share_array[j].plant_name=%s,%d,%d\n",plant_name,cost_share_array[j].plant_name,strlen(plant_name),strlen(cost_share_array[j].plant_name));
			if(0==strcmp(plant_name,(*cost_share_array)[j].plant_name))
			{	
				//printf("\n���%f\n",atof(check_cost));
							
				(*cost_share_array)[j].check_cost += (atof(check_cost)*COST_PER_MARK);
				(*cost_collect17)[j].value[11] += (atof(check_cost));
				//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
				break;
			}
		}
		
	}
	printf("ֻ�мƻ����߿���ʱ:��������Ŀ%d\n",*plant_num);
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
		PERROR_CLOSEDATABASE_EXIT(-1,"get_unplan_shutdown_result()��exec_sql()")

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
		flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()��dm_fetch_scroll()")
		else if(DM_NO_DATA_FOUND==flag)
		{
			/*fprintf(stderr,"%sִ��ʱ,���ݱ���û������,get_unplan_shutdown_result()��dm_fetch_scroll()��,��%d��ѭ��,��%d,�ļ�%s\n",
				sql_string,i,__LINE__,__FILE__);*/
			PERROR_CLOSEDATABASE_EXIT(-1,"ִ��sqlʱ,���ݱ���û������,get_cost_result()��dm_fetch_scroll()")
		}
		for(j=0;j<*plant_num;j++)
		{
			//printf("%s,%s,i=%d,j=%d\n",plant_name,check_cost,i,j);	
			//printf("plant_name=%s,cost_share_array[j].plant_name=%s,%d,%d\n",plant_name,cost_share_array[j].plant_name,strlen(plant_name),strlen(cost_share_array[j].plant_name));
			if(0==strcmp(plant_name,(*cost_share_array)[j].plant_name))
			{	
				//printf("\n���%f\n",atof(check_cost));
							
				(*cost_share_array)[j].check_cost+=(atof(check_cost)*COST_PER_MARK);
				(*cost_collect17)[j].value[5] += (atof(check_cost));
				//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
				break;
			}
		}
		
	}
	printf("�����ͣ���˽����:��������Ŀ%d\n",*plant_num);
	printf("����������     ,���̿��˷���\n");
	for(i=0;i<*plant_num;i++)
	{
		printf("%s,     %f\n",(*cost_share_array)[i].plant_name,(*cost_share_array)[i].check_cost);
	}
	

	printf("�����ͣ���˽���ͼƻ����߿��˽����:��������Ŀ%d\n",*plant_num);
	printf("����������     ,���̿��˷���\n");
	for(i=0;i<*plant_num;i++)
	{
		for(j=0;j<COLLECT_NUM;j++)
		printf("%-20s     %-20s		%-16.6f\n",(*cost_collect17)[i].plant_name,cost_collect_order[j],(*cost_collect17)[i].value[j]);
		
	}



//=========================================================================================
//������й������֣���дcost_share_stru�ṹ����
//select sname ,sum(svalue)from plan.plan.run_manage_check where sdate<'2013-05-32'and sdate>'2013-05-00' group by sname
//=========================================================================================

	sprintf(sql_string,"select sname ,sum(svalue)from\
	plan.plan.run_manage_check where sdate<'%s-32'and\
	sdate>'%s-00' group by sname",month,month);

	if (exec_sql(sql_string) != 0)
		PERROR_CLOSEDATABASE_EXIT(-1,"ִ��sqlʱ,���ݱ���û������,get_cost_result()��exec_sql()")

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
			/*fprintf(stderr,"%sִ��ʱ,���ݱ���û������,get_unplan_shutdown_result()��dm_fetch_scroll()��,��%d��ѭ��,��%d,�ļ�%s\n",
				sql_string,i,__LINE__,__FILE__);*/
			PERROR_CLOSEDATABASE_EXIT(-1,"ִ��sqlʱ,���ݱ���û������,get_cost_result()��dm_fetch_scroll()")
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
	printf("�������п��˽����:��������Ŀ%d\n",*plant_num);
	printf("����������     ,���̿��˷���\n");
	for(i=0;i<*plant_num;i++)
	{
		printf("%s,     %f\n",(*cost_share_array)[i].plant_name,(*cost_share_array)[i].check_cost);
	}


	
	//=========================================================================================
	//������й���������ʮ�����дcost_collect_stru�ṹ����
	//select sname ,sum(svalue)from plan.plan.run_manage_check where sdate<'2013-05-32'and sdate>'2013-05-00' group by sname
	//=========================================================================================

	/*select sname,sfield,sum(svalue) 
	from plan.run_manage_check 
	where sdate like '2013-05-%%' group by sname,sfield
	*/
	/*
	char plant_name[256];
		double agc_service; 		 //AGC����1
		double security_manage; 	 //��ȫ����2
		double high_voltage_device;  //�糧��ѹ������豸3
		double dispatch_manage; 	 //����ҵ�����4
		double peak_load_regulation; //�������5
		double unplan_shutdown_check;//�Ǽƻ�ͣ������6
		double wind_power_forcast;	 //���Ԥ�⿼��7
		double black_start_service;  //����������8
		double unit_maintenance;	 //�������9
		double relay_protection;	 //�̵籣��10
		double excitation_pss_manage;//���ź�pssװ�ù���11
		double curve_default_check; //����ΥԼ����12
		double reservoir_operation; //ˮ�����13
		double communication;		//ͨ��14
		double reactive_regulation_service;//�޹����ڷ���15
		double onetime_frequency_modulation;//һ�ε�Ƶ����16
		double automation;			//�Զ���17
	
	*/
	sprintf(sql_string,"select sname,sfield,sum(svalue)" 
	" from plan.plan.run_manage_check "
	"where sdate < '%s-32' and sdate> '%s-00' group by sname,sfield",month,month);

	printf("\n\ntai yang %s\n",sql_string);
	
	if (exec_sql(sql_string) != 0)
		PERROR_CLOSEDATABASE_EXIT(-1,"ִ��sqlʱ,���ݱ���û������,get_cost_result()��exec_sql()")

	if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
		plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()��dm_bind_column()")

	if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_CHAR,TYPE_DEFAULT,
		field,sizeof(field),&field_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()��dm_bind_column()")

	if(DM_FALSE==dm_bind_column(hsmt,3,DM_DATA_DOUBLE,TYPE_DEFAULT,
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
			/*fprintf(stderr,"%sִ��ʱ,���ݱ���û������,get_unplan_shutdown_result()��dm_fetch_scroll()��,��%d��ѭ��,��%d,�ļ�%s\n",
				sql_string,i,__LINE__,__FILE__);*/
			PERROR_CLOSEDATABASE_EXIT(-1,"ִ��sqlʱ,���ݱ���û������,get_cost_result()��dm_fetch_scroll()")
		}
		printf("%-20s	%-20s	%-16.6f\n",plant_name,field,atof(check_cost));		
		for(j=0;j<*plant_num;j++)
		{
			if(0==strcmp(plant_name,(*cost_collect17)[j].plant_name))
			{	
				for(k=0;k<COLLECT_NUM;k++)
				{
					if(!strcmp(cost_collect_order[k],field))
					{
						(*cost_collect17)[j].value[j] += atof(check_cost);
					}
				}


				//printf("\n���%f\n",atof(check_cost));
							
				(*cost_share_array)[j].check_cost+=(atof(check_cost)*COST_PER_MARK);
				//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
				break;
			}
		}
		
	}


	printf("�����ͣ���˽���ͼƻ����߿��˽����ʮ�����:��������Ŀ%d\n",*plant_num);
	printf("����������     ,���̿��˷���\n");
	for(i=0;i<*plant_num;i++)
	{
		for(j=0;j<COLLECT_NUM;j++)
		printf("%-20s     %-20s		%-16.6f\n",(*cost_collect17)[i].plant_name,cost_collect_order[j],(*cost_collect17)[i].value[j]);
		
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
			PERROR_CLOSEDATABASE_EXIT(-1,"get_month_electricity()��exec_sql()")

		if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
			plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
			PERROR_CLOSEDATABASE_EXIT(-1,"get_month_electricity()��dm_bind_column()")

		if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_DOUBLE,TYPE_DEFAULT,
			month_power,sizeof(month_power),&month_power_size,DM_DESC_PTR_LEN_DEFAULT))
			PERROR_CLOSEDATABASE_EXIT(-1,"get_month_electricity()��dm_bind_column()")
	
		if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
			PERROR_CLOSEDATABASE_EXIT(-1,"get_month_electricity()��dm_describe_row_num()")

	
		for(i=0;i<row_num;i++)
		{
			flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
			if(DM_FALSE==flag)
				PERROR_CLOSEDATABASE_EXIT(-1,"get_month_electricity()��dm_fetch_scroll()")

			else if(DM_NO_DATA_FOUND==flag)
			{
				/*fprintf(stderr,"%sִ��ʱ,���ݱ���û������,get_month_electricity()��dm_fetch_scroll()��,��%d��ѭ��,��%d,�ļ�%s\n",
					sql_string,i,__LINE__,__FILE__);*/
				PERROR_CLOSEDATABASE_EXIT(-1,"ִ��sqlʱ,���ݱ���û������,get_month_electricity()��dm_fetch_scroll()")
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
/*
*����:�������п��˷��÷�̯�ӳ���:���ܽ����д���ݿ�
*����:����洢��Ϣ�ṹ��ͽṹ�嵥Ԫ��Ŀ
*����ֵ:-1����ʧ�ܣ�0��������Ч���������ݿ�
*
*/
int write_share_stru(char* month,cost_share_stru **cost_share_array, cost_collect_stru **cost_collect17, long long* plant_num)
{
	double    sum_month_electricity=0,sum_check_cost=0,sum_accout_cost=0;
	long      long i, j;
	char      sql_string[256];
	size_t    num=200;//��ÿ��select ������Ŀռ�
	long long num_of_insert=0;
	//���㿼���ֺܷ��¶�����������
	if(!strcmp(para.type,"share"))
	{
		for(i=0;i<*plant_num;i++)
		{
			sum_month_electricity += (*cost_share_array)[i].month_electricity;
			sum_check_cost += (*cost_share_array)[i].check_cost;
		}
		//���ṹ����д��̯���úͽ������
		for(i=0;i<*plant_num;i++)
		{
			(*cost_share_array)[i].share_cost = (*cost_share_array)[i].month_electricity / sum_month_electricity*sum_check_cost;
			(*cost_share_array)[i].account_cost = (*cost_share_array)[i].share_cost - (*cost_share_array)[i].check_cost;
		}
		for(i=0;i<*plant_num;i++)
		{
			sum_accout_cost += (*cost_share_array)[i].account_cost;
		}
		if(abs(sum_accout_cost) > 0.001)
			printf("\n\n\n��Ϊ�������!%f\n\n\n",sum_accout_cost);

		sprintf(sql_string,"delete from plan.plan.run_check_cost_share where sdate='%s'",month);
		if (exec_sql(sql_string) != 0)
			PERROR_CLOSEDATABASE_EXIT(-1,"write_share_stru()��exec_sql()ɾ����ǰ����")

		
		

		for(i=0;i<*plant_num;i++)
		{
			sprintf(sql_string,"insert into plan.plan.run_check_cost_share values('%s', '%s', '%f', '%f', '%f', '%f')",
				(*cost_share_array)[i].year_month,(*cost_share_array)[i].plant_name,
				(*cost_share_array)[i].month_electricity,(*cost_share_array)[i].check_cost,
				(*cost_share_array)[i].share_cost,(*cost_share_array)[i].account_cost);
			//printf("\n\n\n\n******%s\n",sql_string);
			if (exec_sql(sql_string) != 0)
			{
				printf("insert error in file: %s , line: %d \n",__FILE__,__LINE__);
				continue;
				//PERROR_CLOSEDATABASE_EXIT(-1,"write_share_stru()��exec_sql()")
			}
			num_of_insert++;
			
		}
		printf("\n\n\n The program has insert %d items\n",num_of_insert);
	}
	else if(!strcmp(para.type,"collect"))
	{
		num_of_insert=0;
		sprintf(sql_string,"delete from plan.plan.run_check_cost_collect where sdate='%s'",month);
		if (exec_sql(sql_string) != 0)
			PERROR_CLOSEDATABASE_EXIT(-1,"write_share_stru()��exec_sql()ɾ����ǰ����")
		

		for(i=0;i<*plant_num;i++)
		{
			for(j=0;j<COLLECT_NUM;j++)
			{
				sprintf(sql_string,"insert into plan.plan.run_check_cost_collect values('%s', '%s', '%s', '%f')",
					month,
					(*cost_collect17)[i].plant_name,
					cost_collect_order[j],
					(*cost_collect17)[i].value[j]
					);
			//	printf("\n******%s\n",sql_string);
				if (exec_sql(sql_string) != 0)
				{
					printf("insert error in file: %s , line: %d \n",__FILE__,__LINE__);
					continue;
					//PERROR_CLOSEDATABASE_EXIT(-1,"write_share_stru()��exec_sql()")
				}
				num_of_insert++;
			}
			
		}
		printf("\n\n\n The program has insert %d items\n",num_of_insert);


		
		printf("�����:��������Ŀ%d\n",*plant_num);
		printf("��  ��,�� �� �� �� ��,�¶������������� �� �� ��,�� ̯ �� �ã��� �� �� ��\n");
		for(i=0;i<*plant_num;i++)
		{
			printf("%s, 	%s,   %f     %f    %f    %f\n",(*cost_share_array)[i].year_month,(*cost_share_array)[i].plant_name,(*cost_share_array)[i].month_electricity,(*cost_share_array)[i].check_cost,(*cost_share_array)[i].share_cost,(*cost_share_array)[i].account_cost);
		}
	}
}



/*
*����:�������п��˷��÷�̯
*����:��Ҫ�������п��˷��÷�̯���·�
*����ֵ:-1����ʧ�ܣ�0��������Ч���������ݿ�
*
*/
int calculate_run_check_cost_share()
{
	cost_share_stru *cost_share_array;
	cost_collect_stru *cost_collect17;
	long long plant_num;


	printf("\n\n\n");
	if(init_database()!=0)
		return(-1);
	
	if(-1==get_unit_info(&cost_share_array,&cost_collect17,para.month,&plant_num))
		{printf("ִ��get_unit_info()ʧ��\n");return -1;}
		
	if(-1==get_cost_result(para.month,&cost_share_array, &cost_collect17, &plant_num))
		{printf("ִ��get_unplan_shutdown_result()ʧ��\n");return -1;}

	
	if(-1==get_month_electricity(para.month,&cost_share_array,&plant_num))
		{printf("ִ��get_month_electricity()ʧ��\n");return -1;}	
		
	if(-1==write_share_stru(para.month,&cost_share_array, &cost_collect17, &plant_num))
		{printf("ִ��write_share_stru()ʧ��\n");return -1;}	

		
	free(cost_share_array);
	write_act_info("�ɹ�","��");
	close_database();
	printf("\n\n");
}

int get_date_and_time(char*in_date,char*in_time)
{

	time_t timep;
	struct tm *p;
	if(-1 == time(&timep))
		perror("get_date_and_time()����");
	p = localtime(&timep);
	if(!p)
		perror("get_date_and_time()����");
	sprintf(in_date,"%04d-%02d-%02d",(1900+p->tm_year), (1+p->tm_mon),p->tm_mday);
	sprintf(in_time,"%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec);//2000/10/28 Sat 8:15:38
	//printf("get_date_and_time() %s  %s\n",in_date,in_time);
}

/*
*����:д����������Ϣ
*����:sid��������
*����ֵ:-1����ʧ�ܣ�0��������Ч���������ݿ�
*
*/
int write_act_info(char *sresult,char *note)
{
	char sql_string[256];
	//char time[30],date[30];
	
	
	get_date_and_time(act_info_to_write.sdate,act_info_to_write.stime);
	sprintf(act_info_to_write.stype,"%s","���п��˷��÷�̯");
	//sprintf(act_info_to_write.sid,"%s",sid);
	sprintf(act_info_to_write.sdesc,"%s","�������п��˷��÷�̯");
	sprintf(act_info_to_write.sresult,"%s",sresult);
	sprintf(act_info_to_write.note,"%s",note);
	sprintf(sql_string,"insert into plan.plan.sys_cmd_act_info "
		"(sdate,stime,stype,sid,sdesc,sresult,note)"
		"values('%s','%s','%s','%s','%s','%s','%s')"
		,act_info_to_write.sdate,act_info_to_write.stime,
		act_info_to_write.stype,act_info_to_write.sid,
		act_info_to_write.sdesc,act_info_to_write.sresult,
		act_info_to_write.note);
	printf("%s\n",sql_string);
	if (exec_sql(sql_string) != 0)
		fprintf(stderr,"%s","write_act_info()��exec_sql()д����������Ϣ����");
	//fwrite(now, strlen(now)+1, 1, fp);
	//sprintf(sql_string,"delete from plan.plan.run_check_cost_share where sdate='%s'",month);
	//	if (exec_sql(sql_string) != 0)
	//		PERROR_CLOSEDATABASE_EXIT(-1,"write_share_stru()��exec_sql()ɾ����ǰ����")
}


/*
*���Դ���
*
*
*
*/
int main(int argc,char **argv)
{
	
	sprintf(act_info_to_write.sid,"%s",argv[0]+2);

	if(argc!=3||(strcmp(argv[1],"share")&&strcmp(argv[1],"collect")))
	{
		printf("please input correct parameter\n");
		printf("Such as ./cal_run_check_cost_share collect 2013-05\n");
		printf("or ./cal_run_check_cost_share share 2013-05\n");
		return -1;
	}
	strcpy(para.type,argv[1]);
	strcpy(para.month,argv[2]);

	if(init_database()!=0)
		return(-1);
	if(-1 == calculate_run_check_cost_share())
		{printf("ִ��calculate_run_check_cost_share()ʧ��\n");return -1;}
	
}


