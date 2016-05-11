/*
*cal_last_result.c
*
*功能:计算运行考核费用分摊和考核汇总
*
*
**/

#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include"api.h"
#include"log.h"
#include"dm_operater.h"
#include"cal_run_check_cost_collect.h"
#include"common.h"

char month[20];
void print_info(int print_num ,cost_collect_stru **cost_collect17, long long *plant_num)
{
	long long i=0,j=0;
	if(-1 == print_num)
	{
		for(i=0;i<*plant_num;i++)
			printf("%s\n",(*cost_collect17)[i].plant_name);
	}
	else if(-2 == print_num)
	{
		for(i=0;i<*plant_num;i++)
		{
			for(j=0;j<COLLECT_NUM;j++)
				printf("%-15s %-20s %-14.4f\n",(*cost_collect17)[i].plant_name,cost_collect_order[j],(*cost_collect17)[i].value[j]);
			
		}
	}
	else if(0 <= print_num)
	{
		for(i=0;i<*plant_num;i++)
		{
			printf("%-20s %-20s %-14.4f\n",(*cost_collect17)[i].plant_name,cost_collect_order[print_num],(*cost_collect17)[i].value[print_num]);
		}
	}
	printf("\n");
}
//=======================================================================================
/*
*功能:从数据库机组信息表unit_info中取电厂名字，动态申请空间，并初始化cost_collect_stru结构
*参数:plant_num用于传出动态申请cost_collect_stru结构的单元数量
*返回值:返回申请空间指针
*/
//======================================================================================
int get_unit_info( cost_collect_stru **cost_collect17,
char *month, long long* plant_num)
{
	char sql_string[256];
	char plant_name[256];
	long plant_name_size;
	long long i, j, row_num;
	int  flag;           //判断dm_fetch_scroll()是否成功

	print_curtline("get_unit_info() start");
	
	sprintf(sql_string, "%s", "select distinct sname from plan.plan.station_info");
	if (exec_sql(sql_string) != 0)
		PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()中exec_sql()")
		
	if (DM_FALSE == dm_bind_column(hsmt, 1, DM_DATA_CHAR, TYPE_DEFAULT, 
		plant_name, sizeof(plant_name), &plant_name_size, DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_unit_info()中dm_bind_column()")


	if(DM_FALSE == dm_describe_row_num(hsmt, plant_num) )
		PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()中dm_describe_row_num()")
	
	if(0 >= (*plant_num))
		PERROR_CLOSEDATABASE_EXIT(-1, "no plant find, get 0 items from data from unit_info")
	
	*cost_collect17 = (cost_collect_stru*)malloc( (*plant_num)*sizeof(cost_collect_stru) );
	if(cost_collect17==NULL)
		PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()中malloc()")
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
			PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()中dm_fetch_scroll()")
		else if(DM_NO_DATA_FOUND == flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"select distinct sname from "
				"plan.plan.month_electricity执行时,数据表中没有数据,get_unit_info()中dm_fetch_scroll()")
				
		strcpy((*cost_collect17)[i].plant_name, plant_name);
	}
	print_info(-1 ,cost_collect17,plant_num);
	print_curtline("get_unit_info() end");
	return 0;
}

//=======================================================================================
/*
功能:从数据库取对应电厂的考核结果
参数:month输入要计算的月份的曲线考核结果
	:cost_collect_array指向 运行考核费用分摊表 结构的指针
	:plant_num表结构中单元数目
返回值:返回-1执行失败，0执行成功
*/
//======================================================================================
int get_cost_result(char *month,cost_collect_stru **cost_collect17, long long* plant_num)
{
	
	long long i,j,k;
	long long row_num;
	char sql_string[512];
	char plant_name[256];
	long plant_name_size;
	char check_cost[20];
	long check_cost_size;
	int  flag;
	char field[22];
	long field_size;

	//===================================================================================	
	/*取得计划考核结果
	*/
	//================================================================================

	printf("\n===================================================get_cost_result()开始=======\n\n");
	sprintf(sql_string,"select sname,sum(result_num) "
					   "from plan.plan.plan_check_result " 
					   "where sstyle='厂站' and "
					   "sdate like '%s-%%%%' group by sname "
					   "union all "
					   "select sstation,sum(result_num) from "
					   "plan.plan.unit_info, plan.plan.plan_check_result "
					   "where plan.plan.unit_info.sname=plan.plan.plan_check_result.sname "
				       "and sdate like '%s-%%%%' and sstyle = '机组' "
					   "group by plan.plan.unit_info.sstation",month,month);
	/*sprintf(sql_string,"select sstation,sum(result_num) from\
		plan.plan.unit_info,plan.plan.plan_check_result\
		where plan.plan.unit_info.sname=plan.plan.plan_check_result.sname\
		and ('%s-00'< sdate and sdate < '%s-32')\
		group by plan.plan.unit_info.sstation",month,month);*/
	printf("sql_string: %s\n",sql_string);
	
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
	
	if(0 >= row_num)
	{
		printf("my_Worning:get_cost_result()中返回结果集(曲线考核,扣分电场为0)出错，结果条目为0\n");
	}
	for(i=0;i<row_num;i++)
	{
		flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_fetch_scroll()")

		else if(DM_NO_DATA_FOUND==flag)
		{
			PERROR_CLOSEDATABASE_EXIT(-1,"执行sql时,数据表中没有数据,get_cost_result()中dm_fetch_scroll()")
		}
		for(j=0;j<*plant_num;j++)
		{
			if(0 == strcmp(plant_name,(*cost_collect17)[j].plant_name))
			{								
				(*cost_collect17)[j].value[11] += (atof(check_cost))*COST_PER_MARK;
				break;
			}
		}
		
	}
	printf("只有计划曲线考核时:工厂的数目%lld\n",*plant_num);
	print_info(11, cost_collect17, plant_num);
	print_curtline("曲线违约考核计算完毕");
//===================================================================================	
/*取得非停考核结果
*/
//================================================================================
	print_curtline("非计划停机考核开始");
	sprintf(sql_string,"select sstation,sum(result_num) from\
		plan.plan.unit_info,plan.plan.unplan_shutdown_result\
		where plan.plan.unit_info.sname=plan.plan.unplan_shutdown_result.sname\
		and ('%s-00'< sdate and sdate < '%s-32')\
		group by plan.plan.unit_info.sstation",month,month);

	printf("\nsql_string: %s\n",sql_string);
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

	if(0 >= row_num)
	{
		printf("my_Worning:get_cost_result()中非停考核结果集返回条目为0\n");
	}

	for(i=0;i<row_num;i++)
	{
		flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_fetch_scroll()")
		else if(DM_NO_DATA_FOUND==flag)
		{
			PERROR_CLOSEDATABASE_EXIT(-1,"执行sql时,数据表中没有数据,get_cost_result()中dm_fetch_scroll()")
		}
		for(j=0;j<*plant_num;j++)
		{
			if(0==strcmp(plant_name,(*cost_collect17)[j].plant_name))
			{	
				(*cost_collect17)[j].value[5] += (atof(check_cost))*COST_PER_MARK;
				break;
			}
		}
		
	}
	printf("加入非停考核结果后:工厂的数目%lld\n",*plant_num);
	
	print_info(5, cost_collect17, plant_num);
	print_curtline("非计划停机考核计算完毕");
	
	
	//=========================================================================================
	//获得运行管理评分中十五项，并写cost_collect_stru结构数组
	//select sname ,sum(svalue)from plan.plan.run_manage_check where sdate<'2013-05-32'and sdate>'2013-05-00' group by sname
	//=========================================================================================
	print_curtline("运行管理评分中十五项计算开始");
	sprintf(sql_string,
			"select sname,sfield,sum(svalue)" 
			" from plan.plan.run_manage_check "			
			"where sdate < '%s-32' and sdate> '%s-00' "			
			"group by sname,sfield",month,month);

	printf("\n\nsql_string: %s\n",sql_string);
	
	if (exec_sql(sql_string) != 0)
		PERROR_CLOSEDATABASE_EXIT(-1,"执行sql时,数据表中没有数据,get_cost_result()中exec_sql()")

	if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
		plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_bind_column()")

	if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_CHAR,TYPE_DEFAULT,
		field,sizeof(field),&field_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_bind_column()")

	if(DM_FALSE==dm_bind_column(hsmt,3,DM_DATA_DOUBLE,TYPE_DEFAULT,
		check_cost,sizeof(check_cost),&check_cost_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_bind_column()")


	if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_describe_row_num()")

	if(0 >= row_num)
		printf("my_Wornding:运送考核汇总时运行管理考核费用返回条目为0\n");
	for(i=0;i<row_num;i++)
	{
		flag=dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()中dm_fetch_scroll()")

		else if(DM_NO_DATA_FOUND==flag)
		{
			PERROR_CLOSEDATABASE_EXIT(-1,"执行sql时,数据表中没有数据,get_cost_result()中dm_fetch_scroll()")
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
						(*cost_collect17)[j].value[k] += atof(check_cost)*COST_PER_MARK;
					}
				}
				break;
			}
		}
		
		
	}

	print_curtline("运行管理考核十五项显示开始");
	
	printf("加入非停考核结果和计划曲线考核结果和十五项后,工厂的数目%lld\n",*plant_num);
	print_info(-2, cost_collect17,plant_num);

	print_curtline("运行管理考核十五项显示结束");
	print_curtline("运行管理考核十五项计算结束");
	return 0;
}

/*
*功能:计算运行考核费用分摊子程序:汇总结果，写数据库
*参数:传入存储信息结构体和结构体单元数目
*返回值:-1计算失败，0计算结果有效并插入数据库
*
*/
int write_collect_stru(char* month,cost_collect_stru **cost_collect17, long long* plant_num)
{
	long      long i, j;
	char      sql_string[256];
	long long num_of_insert=0;
	//计算考核总分和月度总上网电量
	print_curtline("write_collect_stru() start");
	sprintf(sql_string,"delete from plan.plan.run_check_cost_collect where sdate='%s'",month);
	if (exec_sql(sql_string) != 0)
		PERROR_CLOSEDATABASE_EXIT(-1,"write_collect_stru()中exec_sql()删除以前数据")
	

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
			}
			num_of_insert++;
		}
		
	}
	printf("\n\n\n The program has insert %lld items\n",num_of_insert);


/*	
	printf("最后结果:工厂的数目%d\n",*plant_num);
	printf("年  月,工 厂 的 名 称,月度上网电量，考 核 费 用,分 摊 费 用，结 算 费 用\n");
	for(i=0;i<*plant_num;i++)
	{
		printf("%s, 	%s,   %f     %f    %f    %f\n",(*cost_collect_array)[i].year_month,(*cost_collect_array)[i].plant_name,(*cost_collect_array)[i].month_electricity,(*cost_collect_array)[i].check_cost,(*cost_collect_array)[i].collect_cost,(*cost_collect_array)[i].account_cost);
	}
	*/
	print_curtline("write_collect_stru() end");
}



/*
*功能:计算运行考核费用分摊
*参数:需要计算运行考核费用分摊的月份
*返回值:-1计算失败，0计算结果有效并插入数据库
*
*/
int calculate_run_check_cost_collect()
{
	cost_collect_stru *cost_collect17;
	long long plant_num;


	printf("\n\n\n");
	if(init_database()!=0)
		return(-1);
	
	if(-1==get_unit_info(&cost_collect17,month,&plant_num))
		{printf("执行get_unit_info()失败\n");return -1;}
	
	if(-1==get_cost_result(month,&cost_collect17, &plant_num))
		{printf("执行get_unplan_shutdown_result()失败\n");return -1;}

	if(-1==write_collect_stru(month, &cost_collect17, &plant_num))
		{printf("执行write_collect_stru()失败\n");return -1;}	

		
	free(cost_collect17);
	write_act_info("成功","无");
	close_database();
	printf("\n\n");
}


/*
*测试代码
*
*
*
*/
int main(int argc,char **argv)
{
	
	sprintf(act_info_to_write.sid,"%s",argv[0]+2);

	if(argc != 2)
	{
		printf("please input correct parameter\n");
		printf("or %s 2013-05\n",argv[0]);
		return 0;
	}
	strcpy(month,argv[1]);

	if(init_database()!=0)
	{printf("init_database failed\n");return(0);}
	if(-1 == calculate_run_check_cost_collect())
		{printf("执行calculate_run_check_cost_collect()失败\n");return 0;}
	//system("pause");
}


