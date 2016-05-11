/*
*cal_last_result.c
*
*功能:计算运行考核费用分摊
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

//程序执行信息
act_info_stru act_info_to_write;


//=======================================================================================
/*
*功能:从数据库机组信息表unit_info中取电厂名字，动态申请空间，并初始化cost_share_stru结构
*参数:plant_num用于传出动态申请cost_share_stru结构的单元数量
*返回值:返回申请空间指针
*/
//======================================================================================
int get_unit_info(cost_share_stru **run_check_cost_share, char *month, long long* plant_num)
{
	char sql_string[256];
	char plant_name[256];
	long plant_name_size;
	long long i, row_num;
	int  flag;           //判断dm_fetch_scroll()是否成功

	printf("\n\n\nhere\n\n\n");
	sprintf(sql_string, "%s", "select distinct sstation from plan.plan.unit_info");
	if (exec_sql(sql_string) != 0)
		PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()中exec_sql()")
		
	if (DM_FALSE == dm_bind_column(hsmt, 1, DM_DATA_CHAR, TYPE_DEFAULT, 
		plant_name, sizeof(plant_name), &plant_name_size, DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_unit_info()中dm_bind_column()")


	if(DM_FALSE == dm_describe_row_num(hsmt, plant_num) )
		PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()中dm_describe_row_num()")

	*run_check_cost_share = (cost_share_stru*)malloc((*plant_num)*sizeof(cost_share_stru) );
	if(run_check_cost_share==NULL)
		PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()中malloc()")
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

	for(i=0;i<*plant_num;i++)
	{
		flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE == flag)
			PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()中dm_fetch_scroll()")
		else if(DM_NO_DATA_FOUND == flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"select distinct sname from "
				"plan.plan.month_electricity执行时,数据表中没有数据,get_unit_info()中dm_fetch_scroll()")
				
		strcpy((*run_check_cost_share)[i].plant_name, plant_name);
	}
	
	printf("\nget_unit_info():\n单元占用空间:%d,一共单元数:%d*%d\n",sizeof(cost_share_stru),*plant_num,sizeof(cost_share_stru));
	for(i=0;i<*plant_num;i++)
	{
		printf("%s\n",(*run_check_cost_share)[i].plant_name);
	}
	printf("\nget_unit_info()结束\n");
	return 0;
}

//=======================================================================================
/*
功能:从数据库取对应电厂的考核结果
参数:month输入要计算的月份的曲线考核结果
	:cost_share_array指向 运行考核费用分摊表 结构的指针
	:plant_num表结构中单元数目
返回值:返回-1执行失败，0执行成功
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
	int  flag;

	//===================================================================================	
	/*取得计划考核结果
	*/
	//================================================================================


	sprintf(sql_string,"select sstation,sum(result_num) from\
		plan.plan.unit_info,plan.plan.plan_check_result\
		where plan.plan.unit_info.sname=plan.plan.plan_check_result.sname\
		and ('%s-00'< sdate and sdate < '%s-32')\
		group by plan.plan.unit_info.sstation",month,month);

	//PERROR_CLOSEDATABASE_RETURN_IF(0,-1,"测试用的")
	//PERROR_CLOSEDATABASE_RETURN_IF(dec,ret,str)
	//PERROR_CLOSEDATABASE_RETURN(ret,str)
	//PERROR_CLOSEDATABASE_RETURN(-1,"测试用的")
	//if(1)
	//	PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中exec_sql()")
	if (exec_sql(sql_string) != 0)	
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中exec_sql()")
	
	if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
		plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_bind_column()")

	if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_DOUBLE,TYPE_DEFAULT,
		check_cost,sizeof(check_cost),&check_cost_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_bind_column()")

	if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_describe_row_num()")

	for(i=0;i<row_num;i++)
	{
		flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_fetch_scroll()")

		else if(DM_NO_DATA_FOUND==flag)
		{
			/*fprintf(stderr,"%s执行时,数据表中没有数据,get_cost_result()中dm_fetch_scroll()处,第%d次循环,行%d,文件%s\n",
				sql_string,i,__LINE__,__FILE__);*/
			PERROR_CLOSEDATABASE_EXIT(-1,"执行sql时,数据表中没有数据,get_cost_result()中dm_fetch_scroll()")
		}
		for(j=0;j<*plant_num;j++)
		{
			//printf("%s,%s,i=%d,j=%d\n",plant_name,check_cost,i,j);	
			//printf("plant_name=%s,cost_share_array[j].plant_name=%s,%d,%d\n",plant_name,cost_share_array[j].plant_name,strlen(plant_name),strlen(cost_share_array[j].plant_name));
			if(0==strcmp(plant_name,(*cost_share_array)[j].plant_name))
			{	
				//printf("\n相等%f\n",atof(check_cost));
							
				(*cost_share_array)[j].check_cost+=(atof(check_cost)*COST_PER_MARK);
				//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
				break;
			}
		}
		
	}
	printf("只有计划曲线考核时:工厂的数目%d\n",*plant_num);
		printf("工厂的名称	   ,工程考核费用\n");
		for(i=0;i<*plant_num;i++)
		{
			printf("%s, 	%f\n",(*cost_share_array)[i].plant_name,(*cost_share_array)[i].check_cost);
		}

//===================================================================================	
/*取得非停考核结果
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
		PERROR_CLOSEDATABASE_EXIT(-1,"get_unplan_shutdown_result()中exec_sql()")

	if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
		plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_bind_column()")

	if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_DOUBLE,TYPE_DEFAULT,
		check_cost,sizeof(check_cost),&check_cost_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_bind_column()")

	if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_describe_row_num()")


	for(i=0;i<row_num;i++)
	{
		flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_fetch_scroll()")
		else if(DM_NO_DATA_FOUND==flag)
		{
			/*fprintf(stderr,"%s执行时,数据表中没有数据,get_unplan_shutdown_result()中dm_fetch_scroll()处,第%d次循环,行%d,文件%s\n",
				sql_string,i,__LINE__,__FILE__);*/
			PERROR_CLOSEDATABASE_EXIT(-1,"执行sql时,数据表中没有数据,get_cost_result()中dm_fetch_scroll()")
		}
		for(j=0;j<*plant_num;j++)
		{
			//printf("%s,%s,i=%d,j=%d\n",plant_name,check_cost,i,j);	
			//printf("plant_name=%s,cost_share_array[j].plant_name=%s,%d,%d\n",plant_name,cost_share_array[j].plant_name,strlen(plant_name),strlen(cost_share_array[j].plant_name));
			if(0==strcmp(plant_name,(*cost_share_array)[j].plant_name))
			{	
				//printf("\n相等%f\n",atof(check_cost));
							
				(*cost_share_array)[j].check_cost+=(atof(check_cost)*COST_PER_MARK);
				//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
				break;
			}
		}
		
	}
	printf("加入非停考核结果后:工厂的数目%d\n",*plant_num);
	printf("工厂的名称     ,工程考核费用\n");
	for(i=0;i<*plant_num;i++)
	{
		printf("%s,     %f\n",(*cost_share_array)[i].plant_name,(*cost_share_array)[i].check_cost);
	}


//=========================================================================================
//获得运行管理评分，并写cost_share_stru结构数组
//select sname ,sum(svalue)from plan.plan.run_manage_check where sdate<'2013-05-32'and sdate>'2013-05-00' group by sname
//=========================================================================================

	sprintf(sql_string,"select sname ,sum(svalue)from\
	plan.plan.run_manage_check where sdate<'%s-32'and\
	sdate>'%s-00' group by sname",month,month);

	if (exec_sql(sql_string) != 0)
		PERROR_CLOSEDATABASE_EXIT(-1,"执行sql时,数据表中没有数据,get_cost_result()中exec_sql()")

	if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
		plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_bind_column()")

	if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_DOUBLE,TYPE_DEFAULT,
		check_cost,sizeof(check_cost),&check_cost_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_bind_column()")


	if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_describe_row_num()")


	for(i=0;i<row_num;i++)
	{
		flag=dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_fetch_scroll()")

		else if(DM_NO_DATA_FOUND==flag)
		{
			/*fprintf(stderr,"%s执行时,数据表中没有数据,get_unplan_shutdown_result()中dm_fetch_scroll()处,第%d次循环,行%d,文件%s\n",
				sql_string,i,__LINE__,__FILE__);*/
			PERROR_CLOSEDATABASE_EXIT(-1,"执行sql时,数据表中没有数据,get_cost_result()中dm_fetch_scroll()")
		}
		for(j=0;j<*plant_num;j++)
		{
			//printf("%s,%s,i=%d,j=%d\n",plant_name,check_cost,i,j);	
			//printf("plant_name=%s,cost_share_array[j].plant_name=%s,%d,%d\n",plant_name,cost_share_array[j].plant_name,strlen(plant_name),strlen(cost_share_array[j].plant_name));
			if(0==strcmp(plant_name,(*cost_share_array)[j].plant_name))
			{	
				//printf("\n相等%f\n",atof(check_cost));
							
				(*cost_share_array)[j].check_cost+=(atof(check_cost)*COST_PER_MARK);
				//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
				break;
			}
		}
		
	}
	printf("加入运行考核结果后:工厂的数目%d\n",*plant_num);
	printf("工厂的名称     ,工程考核费用\n");
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
			PERROR_CLOSEDATABASE_EXIT(-1,"get_month_electricity()中exec_sql()")

		if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
			plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
			PERROR_CLOSEDATABASE_EXIT(-1,"get_month_electricity()中dm_bind_column()")

		if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_DOUBLE,TYPE_DEFAULT,
			month_power,sizeof(month_power),&month_power_size,DM_DESC_PTR_LEN_DEFAULT))
			PERROR_CLOSEDATABASE_EXIT(-1,"get_month_electricity()中dm_bind_column()")
	
		if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
			PERROR_CLOSEDATABASE_EXIT(-1,"get_month_electricity()中dm_describe_row_num()")

	
		for(i=0;i<row_num;i++)
		{
			flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
			if(DM_FALSE==flag)
				PERROR_CLOSEDATABASE_EXIT(-1,"get_month_electricity()中dm_fetch_scroll()")

			else if(DM_NO_DATA_FOUND==flag)
			{
				/*fprintf(stderr,"%s执行时,数据表中没有数据,get_month_electricity()中dm_fetch_scroll()处,第%d次循环,行%d,文件%s\n",
					sql_string,i,__LINE__,__FILE__);*/
				PERROR_CLOSEDATABASE_EXIT(-1,"执行sql时,数据表中没有数据,get_month_electricity()中dm_fetch_scroll()")
			}
			printf("month_power=%f\n",atof(month_power));
			for(j=0;j<*plant_num;j++)
			{
				//printf("%s,%s,i=%d,j=%d\n",plant_name,check_cost,i,j);	
				//printf("plant_name=%s,cost_share_array[j].plant_name=%s,%d,%d\n",plant_name,cost_share_array[j].plant_name,strlen(plant_name),strlen(cost_share_array[j].plant_name));
				if(0==strcmp(plant_name,(*cost_share_array)[j].plant_name))
				{	
					//printf("\n相等%f\n",atof(check_cost));
								
					(*cost_share_array)[j].month_electricity+=atof(month_power);
					
					//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
					break;
				}
			}
			
		}
		printf("加入月度上网电量:工厂的数目%d\n",*plant_num);
		printf("工厂的名称	   ,工程考核费用\n");
		for(i=0;i<*plant_num;i++)
		{
			printf("%s, 	%f,     %f\n",(*cost_share_array)[i].plant_name,(*cost_share_array)[i].check_cost,(*cost_share_array)[i].month_electricity);
		}
}
/*
*功能:计算运行考核费用分摊子程序:汇总结果，写数据库
*参数:传入存储信息结构体和结构体单元数目
*返回值:-1计算失败，0计算结果有效并插入数据库
*
*/
int write_share_stru(char* month,cost_share_stru **cost_share_array,long long* plant_num)
{
	double    sum_month_electricity=0,sum_check_cost=0,sum_accout_cost=0;
	long      long i;
	char      sql_string[256];
	size_t    num=200;//给每条select 语句留的空间
	long long num_of_insert=0;
	//计算考核总分和月度总上网电量
	
	for(i=0;i<*plant_num;i++)
	{
		sum_month_electricity += (*cost_share_array)[i].month_electricity;
		sum_check_cost += (*cost_share_array)[i].check_cost;
	}
	//往结构体中写分摊费用和结算费用
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
		printf("\n\n\n余额不为零很严重!%f\n\n\n",sum_accout_cost);

	sprintf(sql_string,"delete from plan.plan.run_check_cost_share where sdate='%s'",month);
	if (exec_sql(sql_string) != 0)
		PERROR_CLOSEDATABASE_EXIT(-1,"write_share_stru()中exec_sql()删除以前数据")

	
	

	for(i=0;i<*plant_num;i++)
	{
		sprintf(sql_string,"insert into plan.plan.run_check_cost_share values('%s', '%s', '%f', '%f', '%f', '%f')",
			(*cost_share_array)[i].year_month,(*cost_share_array)[i].plant_name,
			(*cost_share_array)[i].month_electricity,(*cost_share_array)[i].check_cost,
			(*cost_share_array)[i].share_cost,(*cost_share_array)[i].account_cost);
		printf("\n\n\n\n******%s\n",sql_string);
		if (exec_sql(sql_string) != 0)
		{
			printf("insert error in file: %s , line: %d \n",__FILE__,__LINE__);
			continue;
			//PERROR_CLOSEDATABASE_EXIT(-1,"write_share_stru()中exec_sql()")
		}
		num_of_insert++;
		
	}
	printf("\n\n\n The program has insert %d items\n",num_of_insert);

	
			
	printf("最后结果:工厂的数目%d\n",*plant_num);
	printf("年  月,工 厂 的 名 称,月度上网电量，考 核 费 用,分 摊 费 用，结 算 费 用\n");
	for(i=0;i<*plant_num;i++)
	{
		printf("%s, 	%s,   %f     %f    %f    %f\n",(*cost_share_array)[i].year_month,(*cost_share_array)[i].plant_name,(*cost_share_array)[i].month_electricity,(*cost_share_array)[i].check_cost,(*cost_share_array)[i].share_cost,(*cost_share_array)[i].account_cost);
	}
	
}



/*
*功能:计算运行考核费用分摊
*参数:需要计算运行考核费用分摊的月份
*返回值:-1计算失败，0计算结果有效并插入数据库
*
*/
int calculate_run_check_cost_share(char* month)
{
	cost_share_stru *cost_share_array;
	long long plant_num;


	printf("\n\n\n");
	if(init_database()!=0)
		return(-1);
	
	if(-1==get_unit_info(&cost_share_array,month,&plant_num))
		{printf("执行get_unit_info()失败\n");return -1;}
		
	if(-1==get_cost_result(month,&cost_share_array,&plant_num))
		{printf("执行get_unplan_shutdown_result()失败\n");return -1;}
		
	if(-1==get_month_electricity(month,&cost_share_array,&plant_num))
		{printf("执行get_month_electricity()失败\n");return -1;}	
		
	if(-1==write_share_stru(month,&cost_share_array,&plant_num))
		{printf("执行write_share_stru()失败\n");return -1;}	

		
	free(cost_share_array);
	write_act_info("成功","无");
	close_database();
	printf("\n\n");
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

/*
*功能:写程序运行信息
*参数:sid程序名称
*返回值:-1计算失败，0计算结果有效并插入数据库
*
*/
int write_act_info(char *sresult,char *note)
{
	char sql_string[256];
	//char time[30],date[30];
	
	
	get_date_and_time(act_info_to_write.sdate,act_info_to_write.stime);
	sprintf(act_info_to_write.stype,"%s","运行考核费用分摊");
	//sprintf(act_info_to_write.sid,"%s",sid);
	sprintf(act_info_to_write.sdesc,"%s","计算运行考核费用分摊");
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
		fprintf(stderr,"%s","write_act_info()中exec_sql()写程序运行信息错误");
	//fwrite(now, strlen(now)+1, 1, fp);
	//sprintf(sql_string,"delete from plan.plan.run_check_cost_share where sdate='%s'",month);
	//	if (exec_sql(sql_string) != 0)
	//		PERROR_CLOSEDATABASE_EXIT(-1,"write_share_stru()中exec_sql()删除以前数据")
}


/*
*测试代码
*
*
*
*/
int main(int argc,char **argv)
{
	char addr[500];
	char file_name[100];
	if(init_database()!=0)
		return(-1);

	getcwd(addr,sizeof(addr));
	sprintf(act_info_to_write.sid,"%s/%s",addr,argv[0]+2);

	if(argc!=2)
	{
		printf("please input correct parameter\n");
		return -1;
	}
	if(-1 == calculate_run_check_cost_share(argv[1]))
		{printf("执行calculate_run_check_cost_share()失败\n");return -1;}	*/
	
}


