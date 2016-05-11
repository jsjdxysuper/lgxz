#include<stdio.h>
#include"api.h"
#include"log.h"
#include"dm_operater.h"
#define COST_PER_MARK 1000	//¿¼·Ö·ÖÖµ¶ÔÓ¦ÈËÃñ±ÒµÄÊıÄ¿
#define  PERROR_CLOSEDATABASE_EXIT_IF(dec,ret,str) if((dec)==DM_FALSE)\
        {fprintf(stderr,"error in: %s,file:%s,line:%d\n",str,__FILE__,__LINE__);close_database();exit(ret);}
#define  PERROR_CLOSEDATABASE_EXIT(ret,str)\
		{fprintf(stderr,"error in: %s,file:%s,line:%d\n",str,__FILE__,__LINE__);close_database();exit(ret);}

//ÓëÔËĞĞ¿¼ºË·ÑÓÃ·ÖÌ¯±írun_check_cost_shareÒ»ÖÂµÄÊı¾İ½á¹¹
typedef struct
{
	char year_month[10];     //ÄêÔÂ
	char plant_name[100];	 //µç³§Ãû³Æ
	double month_electricity;//ÔÂ¶ÈÉÏÍøµçÁ¿
	double check_cost;		 //¿¼ºË·ÑÓÃ
	double share_cost;		 //·ÖÌ¯·ÑÓÃ
	double account_cost;	 //½áËã·ÑÓÃ
}cost_share_stru;

//=======================================================================================
/*
¹¦ÄÜ:´ÓÊı¾İ¿â»ú×éĞÅÏ¢±íunit_infoÖĞÈ¡µç³§Ãû×Ö£¬¶¯Ì¬ÉêÇë¿Õ¼ä£¬²¢³õÊ¼»¯cost_share_stru½á¹¹
²ÎÊı:plant_numÓÃÓÚ´«³ö¶¯Ì¬ÉêÇëcost_share_stru½á¹¹µÄµ¥ÔªÊıÁ¿
·µ»ØÖµ:·µ»ØÉêÇë¿Õ¼äÖ¸Õë
*/
//======================================================================================
cost_share_stru * get_unit_info(char *month,long long* plant_num)
{
	char sql_string[256];
	char plant_name[256];
	long plant_name_size;
	long long i,row_num;
	cost_share_stru *run_check_cost_share;
	int flag;//ÅĞ¶Ïdm_fetch_scroll()ÊÇ·ñ³É¹¦
	sprintf(sql_string,"%s","select distinct sstation from plan.plan.unit_info");

	if (exec_sql(sql_string) != 0)
	{
		fprintf(stderr,"get_unit_info()ÖĞexec_sql()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
		close_database();
		return(NULL);
	}
	if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
	{
		fprintf(stderr,"get_unit_info()ÖĞdm_bind_column()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
		close_database();
		return(NULL);
	}

	if(DM_FALSE==dm_describe_row_num(hsmt,plant_num))
	{
		fprintf(stderr,"get_unit_info()ÖĞdm_bind_column()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
		close_database();
		return(NULL);
	}

	run_check_cost_share=(cost_share_stru*)malloc((*plant_num)*sizeof(cost_share_stru));
	if(run_check_cost_share==NULL)
	{
		fprintf(stderr,"get_unit_info()ÖĞmalloc()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
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
			fprintf(stderr,"get_unit_info()ÖĞdm_fetch_scroll()´¦³ö´í,µÚ%d´ÎÑ­»·,ĞĞ%d,ÎÄ¼ş%s\n",i,__LINE__,__FILE__);
			close_database();
			return(NULL);
		}
		else if(DM_NO_DATA_FOUND==flag)
		{
			fprintf(stderr,"select distinct sname from plan.plan.month_electricityÖ´ĞĞÊ±,Êı¾İ±íÖĞÃ»ÓĞÊı¾İ,get_unit_info()ÖĞdm_fetch_scroll()´¦,µÚ%d´ÎÑ­»·,ĞĞ%d,ÎÄ¼ş%s\n",i,__LINE__,__FILE__);
			close_database();
			return(NULL);
		}
		strcpy(run_check_cost_share[i].plant_name,plant_name);
	}
	
	printf("\nget_unit_info():\nµ¥ÔªÕ¼ÓÃ¿Õ¼ä:%d,Ò»¹²µ¥ÔªÊı:%d*%d\n",sizeof(cost_share_stru),*plant_num,sizeof(cost_share_stru));
	for(i=0;i<*plant_num;i++)
	{
		printf("%s\n",run_check_cost_share[i].plant_name);
	}
	printf("\nget_unit_info()½áÊø\n");
	return run_check_cost_share;
}

//=======================================================================================
/*
¹¦ÄÜ:´ÓÊı¾İ¿âÈ¡¶ÔÓ¦µç³§µÄ¿¼ºË½á¹û
²ÎÊı:monthÊäÈëÒª¼ÆËãµÄÔÂ·İµÄÇúÏß¿¼ºË½á¹û
	:cost_share_arrayÖ¸Ïò ÔËĞĞ¿¼ºË·ÑÓÃ·ÖÌ¯±í ½á¹¹µÄÖ¸Õë
	:plant_num±í½á¹¹ÖĞµ¥ÔªÊıÄ¿
·µ»ØÖµ:·µ»Ø-1Ö´ĞĞÊ§°Ü£¬0Ö´ĞĞ³É¹¦
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
	/*È¡µÃ¼Æ»®¿¼ºË½á¹û
	*/
	//================================================================================


	sprintf(sql_string,"select sstation,sum(result_num) from\
		plan.plan.unit_info,plan.plan.plan_check_result\
		where plan.plan.unit_info.sname=plan.plan.plan_check_result.sname\
		and ('%s-00'< sdate and sdate < '%s-32')\
		group by plan.plan.unit_info.sstation",month,month);

	//PERROR_CLOSEDATABASE_RETURN_IF(0,-1,"²âÊÔÓÃµÄ")
	//PERROR_CLOSEDATABASE_RETURN_IF(dec,ret,str)
	//PERROR_CLOSEDATABASE_RETURN(ret,str)
	//PERROR_CLOSEDATABASE_RETURN(-1,"²âÊÔÓÃµÄ")
	//if(1)
	//	PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()ÖĞexec_sql()")
	if (exec_sql(sql_string) != 0)	
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()ÖĞexec_sql()")
	
	if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
		plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()ÖĞdm_bind_column()")

	if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_DOUBLE,TYPE_DEFAULT,
		check_cost,sizeof(check_cost),&check_cost_size,DM_DESC_PTR_LEN_DEFAULT))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()ÖĞdm_bind_column()")

	if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
		PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()ÖĞdm_describe_row_num()")

	for(i=0;i<row_num;i++)
	{
		flag=dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"get_cost_result()ÖĞdm_fetch_scroll()")

		else if(DM_NO_DATA_FOUND==flag)
		{
			/*fprintf(stderr,"%sÖ´ĞĞÊ±,Êı¾İ±íÖĞÃ»ÓĞÊı¾İ,get_cost_result()ÖĞdm_fetch_scroll()´¦,µÚ%d´ÎÑ­»·,ĞĞ%d,ÎÄ¼ş%s\n",
				sql_string,i,__LINE__,__FILE__);*/
			PERROR_CLOSEDATABASE_EXIT(-1,"´ĞĞÊ±,Êı¾İ±íÖĞÃ»ÓĞÊı¾İ,get_cost_result()ÖĞdm_fetch_scroll()")
		}
		for(j=0;j<*plant_num;j++)
		{
			//printf("%s,%s,i=%d,j=%d\n",plant_name,check_cost,i,j);	
			//printf("plant_name=%s,cost_share_array[j].plant_name=%s,%d,%d\n",plant_name,cost_share_array[j].plant_name,strlen(plant_name),strlen(cost_share_array[j].plant_name));
			if(0==strcmp(plant_name,(*cost_share_array)[j].plant_name))
			{	
				//printf("\nÏàµÈ%f\n",atof(check_cost));
							
				(*cost_share_array)[j].check_cost+=(atof(check_cost)*COST_PER_MARK);
				//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
				break;
			}
		}
		
	}
	printf("Ö»ÓĞ¼Æ»®ÇúÏß¿¼ºËÊ±:¹¤³§µÄÊıÄ¿%d\n",plant_num);
		printf("¹¤³§µÄÃû³Æ	   ,¹¤³Ì¿¼ºË·ÑÓÃ\n");
		for(i=0;i<*plant_num;i++)
		{
			printf("%s, 	%f\n",(*cost_share_array)[i].plant_name,(*cost_share_array)[i].check_cost);
		}

//===================================================================================	
/*È¡µÃ·ÇÍ£¿¼ºË½á¹û
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
		fprintf(stderr,"get_unplan_shutdown_result()ÖĞexec_sql()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}
	if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
		plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
	{
		fprintf(stderr,"get_unplan_shutdown_result()ÖĞdm_bind_column()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}		
	if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_DOUBLE,TYPE_DEFAULT,
		check_cost,sizeof(check_cost),&check_cost_size,DM_DESC_PTR_LEN_DEFAULT))
	{
		fprintf(stderr,"get_unplan_shutdown_result()ÖĞdm_bind_column()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}	

	if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
	{
		fprintf(stderr,"get_unplan_shutdown_result()ÖĞdm_describe_row_num()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}

	for(i=0;i<row_num;i++)
	{
		flag=dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
		{
			fprintf(stderr,"get_unplan_shutdown_result()ÖĞdm_fetch_scroll()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
			close_database();
			return(-1);
		}
		else if(DM_NO_DATA_FOUND==flag)
		{
			fprintf(stderr,"%sÖ´ĞĞÊ±,Êı¾İ±íÖĞÃ»ÓĞÊı¾İ,get_unplan_shutdown_result()ÖĞdm_fetch_scroll()´¦,µÚ%d´ÎÑ­»·,ĞĞ%d,ÎÄ¼ş%s\n",
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
				//printf("\nÏàµÈ%f\n",atof(check_cost));
							
				(*cost_share_array)[j].check_cost+=(atof(check_cost)*COST_PER_MARK);
				//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
				break;
			}
		}
		
	}
	printf("¼ÓÈë·ÇÍ£¿¼ºË½á¹ûºó:¹¤³§µÄÊıÄ¿%d\n",plant_num);
	printf("¹¤³§µÄÃû³Æ     ,¹¤³Ì¿¼ºË·ÑÓÃ\n");
	for(i=0;i<*plant_num;i++)
	{
		printf("%s,     %f\n",(*cost_share_array)[i].plant_name,(*cost_share_array)[i].check_cost);
	}


//=========================================================================================
//»ñµÃÔËĞĞ¹ÜÀíÆÀ·Ö£¬²¢Ğ´cost_share_stru½á¹¹Êı×é
//select sname ,sum(svalue)from plan.plan.run_manage_check where sdate<'2013-05-32'and sdate>'2013-05-00' group by sname
//=========================================================================================

	sprintf(sql_string,"select sname ,sum(svalue)from\
	plan.plan.run_manage_check where sdate<'%s-32'and\
	sdate>'%s-00' group by sname",month,month);

	if (exec_sql(sql_string) != 0)
	{
		fprintf(stderr,"get_unplan_shutdown_result()ÖĞexec_sql()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}
	if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
		plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
	{
		fprintf(stderr,"get_unplan_shutdown_result()ÖĞdm_bind_column()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}		
	if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_DOUBLE,TYPE_DEFAULT,
		check_cost,sizeof(check_cost),&check_cost_size,DM_DESC_PTR_LEN_DEFAULT))
	{
		fprintf(stderr,"get_unplan_shutdown_result()ÖĞdm_bind_column()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}	

	if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
	{
		fprintf(stderr,"get_unplan_shutdown_result()ÖĞdm_describe_row_num()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
		close_database();
		return(-1);
	}

	for(i=0;i<row_num;i++)
	{
		flag=dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE==flag)
		{
			fprintf(stderr,"get_unplan_shutdown_result()ÖĞdm_fetch_scroll()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
			close_database();
			return(-1);
		}
		else if(DM_NO_DATA_FOUND==flag)
		{
			fprintf(stderr,"%sÖ´ĞĞÊ±,Êı¾İ±íÖĞÃ»ÓĞÊı¾İ,get_unplan_shutdown_result()ÖĞdm_fetch_scroll()´¦,µÚ%d´ÎÑ­»·,ĞĞ%d,ÎÄ¼ş%s\n",
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
				//printf("\nÏàµÈ%f\n",atof(check_cost));
							
				(*cost_share_array)[j].check_cost+=(atof(check_cost)*COST_PER_MARK);
				//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
				break;
			}
		}
		
	}
	printf("¼ÓÈëÔËĞĞ¿¼ºË½á¹ûºó:¹¤³§µÄÊıÄ¿%d\n",plant_num);
	printf("¹¤³§µÄÃû³Æ     ,¹¤³Ì¿¼ºË·ÑÓÃ\n");
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
			fprintf(stderr,"get_month_electricity()ÖĞexec_sql()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
			close_database();
			return(-1);
		}
		if(DM_FALSE==dm_bind_column(hsmt,1,DM_DATA_CHAR,TYPE_DEFAULT,
			plant_name,sizeof(plant_name),&plant_name_size,DM_DESC_PTR_LEN_DEFAULT))
		{
			fprintf(stderr,"get_month_electricity()ÖĞdm_bind_column()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
			close_database();
			return(-1);
		}		
		if(DM_FALSE==dm_bind_column(hsmt,2,DM_DATA_DOUBLE,TYPE_DEFAULT,
			month_power,sizeof(month_power),&month_power_size,DM_DESC_PTR_LEN_DEFAULT))
		{
			fprintf(stderr,"get_month_electricity()ÖĞdm_bind_column()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
			close_database();
			return(-1);
		}	
	
		if(DM_FALSE==dm_describe_row_num(hsmt,&row_num))
		{
			fprintf(stderr,"get_month_electricity()ÖĞdm_describe_row_num()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
			close_database();
			return(-1);
		}
	
		for(i=0;i<row_num;i++)
		{
			flag=dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
			if(DM_FALSE==flag)
			{
				fprintf(stderr,"get_month_electricity()ÖĞdm_fetch_scroll()´¦³ö´í,ĞĞ%d,ÎÄ¼ş%s\n",__LINE__,__FILE__);
				close_database();
				return(-1);
			}
			else if(DM_NO_DATA_FOUND==flag)
			{
				fprintf(stderr,"%sÖ´ĞĞÊ±,Êı¾İ±íÖĞÃ»ÓĞÊı¾İ,get_month_electricity()ÖĞdm_fetch_scroll()´¦,µÚ%d´ÎÑ­»·,ĞĞ%d,ÎÄ¼ş%s\n",
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
					//printf("\nÏàµÈ%f\n",atof(check_cost));
								
					(*cost_share_array)[j].month_electricity+=atof(month_power);
					
					//printf("\ncost_share_array[j].check_cost=%f,(atof(check_cost)=%f\n",cost_share_array[j].check_cost,atof(check_cost));
					break;
				}
			}
			
		}
		printf("¼ÓÈëÔÂ¶ÈÉÏÍøµçÁ¿:¹¤³§µÄÊıÄ¿%d\n",*plant_num);
		printf("¹¤³§µÄÃû³Æ	   ,¹¤³Ì¿¼ºË·ÑÓÃ\n");
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
	//¼ÆËã¿¼ºË×Ü·ÖºÍÔÂ¶È×ÜÉÏÍøµçÁ¿
	for(i=0;i<*plant_num;i++)
	{
		sum_month_electricity+=(*cost_share_array)[i].month_electricity;
		sum_check_cost+=(*cost_share_array)[i].check_cost;
	}
	//Íù½á¹¹ÌåÖĞĞ´·ÖÌ¯·ÑÓÃºÍ½áËã·ÑÓÃ
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
		printf("\n\n\nÓà¶î²»ÎªÁãºÜÑÏÖØ!%f\n\n\n",sum_accout_cost);


	printf("×îºó½á¹û:¹¤³§µÄÊıÄ¿%d\n",*plant_num);
	printf("Äê  ÔÂ,¹¤ ³§ µÄ Ãû ³Æ,ÔÂ¶ÈÉÏÍøµçÁ¿£¬¿¼ ºË ·Ñ ÓÃ,·Ö Ì¯ ·Ñ ÓÃ£¬½á Ëã ·Ñ ÓÃ\n");
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
		{printf("Ö´ĞĞget_unplan_shutdown_result()Ê§°Ü\n");return -1;}


	if(-1==get_month_electricity(argv[1],&cost_share_array,&plant_num))
		{printf("Ö´ĞĞget_month_electricity()Ê§°Ü\n");return -1;}
	
	if(-1==write_share_stru(&cost_share_array,&plant_num))
		{printf("Ö´ĞĞwrite_share_stru()Ê§°Ü\n");return -1;}




	
	free(cost_share_array);
	close_database();
	printf("\n\n");
}


