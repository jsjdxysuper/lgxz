#include "dm_operater.h"


/********************************************
* note: init database connection
* return: false: -1, -2,-3,-4; true: 0
********************************************/
int init_database()
{
	// get server ip, usr, pwd
	void *fpini = IniOpenFile(INI_PATH, 0);
    if(fpini == NULL)
    {
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "read ini file failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		printf(buffer);
		printf("read ini file failed!\n");
		return -1;
    }        
  
    IniGetString("DB", "ip", ipstr, MAXSIZE, fpini);
    printf("ip:%s\n", ipstr);

	IniGetString("DB", "usr", usrstr, MAXSIZE, fpini);
    printf("usr:%s\n", usrstr);

	IniGetString("DB", "pwd", pwdstr, MAXSIZE, fpini);
    printf("pwd:%s\n", pwdstr);

	free(fpini);
	
	//LOG BEGIN
	memset(buffer, 0 ,sizeof(buffer));
	sprintf(buffer, "%s\n", "init database connection");
	write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
	
	//create API environment
	printf("create API environment\n");
	sret = dm_api_init();
	if(!(sret == DM_OK || sret == DM_SUCCESS_WITH_INFO))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "create API environment failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		printf(buffer);
		return -1;
	}

	// alloc environment handle
	printf("alloc enironment handle\n");
	if(!dm_alloc_env(&henv))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "alloc environment handle failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		printf(buffer);
		return -2;
	}

	//alloc connection handle
	printf("alloc connection handle\n");
	if(!dm_alloc_connect(henv, &hdbc))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer,  "alloc connection handle failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		printf(buffer);
		dm_free_env(henv);
		return -3;
	}

	//connect localhost
  	printf("connect localhost\n");
	if(!dm_login_port(hdbc, ipstr, usrstr, pwdstr, 12345))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "connect localhost failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		printf(buffer);
		dm_free_connect(hdbc);
		dm_free_env(henv);
		return -4;
	}
	return 0;
}

 
/**********************************************************************
* note: close database connection
* return: true 0;
**********************************************************************/
int close_database()
{
	//free statement handle
	printf("free statement handle\n");
	if(!dm_free_stmt(hsmt))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "free statement handle failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
	}

	//disconnect DM
	printf("disconnect DM\n");
	if(!dm_logout(hdbc))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "disconnect DM failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
	}

	//free connection handle
 	printf("free connection handle\n");
	if(!dm_free_connect(hdbc))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "free connection handle failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
	}

	//free environment handle
	printf("free environment handle\n");
	if(!dm_free_env(henv))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "free environment handle failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
	}
	return 0;
}


/********************************************************************
* note: exe sql
* @param: sql
* return: false -1, -2; true 0;
********************************************************************/
int exec_sql(char* sql)
{
	//alloc statement handle
	//printf("alloc statement handle\n");
	if(!dm_alloc_stmt(hdbc, &hsmt))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "alloc statement handle failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		printf(buffer);
		dm_logout(hdbc);
		dm_free_connect(hdbc);
		dm_free_env(henv);
		return -1;
	}

	//execute procedure
	printf("execute sql\n");
	printf("sql:%s\n", sql);
	if(!dm_direct_exec(hsmt, sql))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "execute sql failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		return -2;
	}	
	return 0;		
}

/****************************************************
* note: write program action
* @param: result, note
****************************************************/
int write_program_action(char* result, char* note)
{
	char sqlstr[512];
	memset(sqlstr, 0, strlen(sqlstr));
	sprintf(sqlstr, 
			"insert into plan.plan.sys_cmd_act_info(sdate,stime,stype,sid,sdesc,sresult,note) "
   			"values('%s','%s','%s','%s','%s','%s','%s')",
   			current_date, current_time, program_type, program_id, program_desc, result, note);
   	if(exec_sql(sqlstr) != 0)
   	{
		printf("write program action failure!\n");
		return -1;
   	}
	return 0;
}

//获取缺省考核指令
int get_check_num()
{
	char check_num[16];
	long cb_check_num = 0;
	int i;
	
	char sqlstr[512];
	memset(sqlstr, 0, strlen(sqlstr));
	sprintf(sqlstr, "select svalue from plan.plan.sys_parameter_config where stype = '曲线考核'  and sname = '缺省考核指令'");  

	if (exec_sql(sqlstr) != 0)
	{
		printf("%s\n", "查询缺省考核指令失败！");
		write_program_action("错误","查询缺省考核指令失败！");
		return -1;
	}

	dm_bind_column(hsmt, 1, DM_DATA_CHAR, TYPE_DEFAULT, check_num, sizeof(check_num), &cb_check_num, DM_DESC_PTR_LEN_DEFAULT);
	
	dm_describe_row_num(hsmt, &row_num);

	if(row_num == 0)
	{
		printf("%s\n", "没有定义缺省考核指令的记录！");
		write_program_action("错误","没有定义缺省考核指令的记录！");
		return -2;
	}

	for (i = 0; i < row_num; ++i)
	{
		dm_fetch_scroll(hsmt, DM_FETCH_NEXT, 0);
		sprintf(default_check_num, check_num, 16);
	}

	printf("默认考核指令：%s\n", default_check_num);

	return 0;
}

//获得曲线考核的机组名称
struct_unit_name *get_curve_check_sname(char* date, int *count)
{
	char sname[512];
	long cbname = 0;
	int i;	
	
	char sqlstr[512];
	memset(sqlstr, 0, strlen(sqlstr));
	sprintf(sqlstr,"select sname from plan.plan.object_config where stype = '曲线考核'");
	char errormsg[512];
	printf("获取机组名称脚本：%s\n", sqlstr);
	if (exec_sql(sqlstr) != 0)
	{
		printf("查询%s天曲线考核机组序列失败!", date);
		memset(errormsg, 0, sizeof(errormsg));
		sprintf(errormsg, "查询%s天曲线考核机组序列失败!", date);
		write_program_action("错误",errormsg);
		return NULL;
	}
	
	dm_bind_column(hsmt, 1, DM_DATA_CHAR, TYPE_DEFAULT, sname, sizeof(sname), &cbname, DM_DESC_PTR_LEN_DEFAULT);
	
	dm_describe_row_num(hsmt, &row_num);
	*count = row_num;
	struct_unit_name *unit_name = (struct_unit_name *)malloc(sizeof(struct_unit_name) * (*count));
	printf("机组记录数：%d\n", row_num);
	
	if(row_num == 0)
	{
		printf("机组记录：%s\n", "曲线考核机组记录数为空！");
		memset(errormsg, 0, sizeof(errormsg));
		sprintf(errormsg, "%s天曲线考核机组序列为空!", date);
		write_program_action("错误", errormsg);
		return NULL;
	}
	
	for (i = 0; i < row_num; ++i)
	{
		dm_fetch_scroll(hsmt, DM_FETCH_NEXT, 0);
		sprintf(unit_name[i].sname, sname, 512);
	}

	return unit_name;
}

 //获取全部时段的考核类型
int get_all_time_check_type(int **unit_check_type, char* date, char* sname, int j)
{
	char time_start[512];
	long cb_time_start = 0;
	char time_end[512];
	long cb_time_end = 0;
	int i;
	int start_time;
	int end_time;

	char sqlstr[512];
	char errormsg[512];
	memset(sqlstr, 0, strlen(sqlstr));
	sprintf(sqlstr,"select time_start,time_end from plan.plan.plan_check_assign where sdate = '%s' and sname = '%s'", date, sname);
	printf("获取全部时段考核类型脚本：%s\n", sqlstr);
	if (exec_sql(sqlstr) != 0)
	{
		printf("%s在%s日的曲线考核记录查询失败\n", sname, date);
		memset(errormsg, 0, sizeof(errormsg));
		sprintf(errormsg, "%s在%s日的曲线考核记录查询失败\n", sname, date);
		write_program_action("错误",errormsg);
		return -1;
	}

	dm_bind_column(hsmt, 1, DM_DATA_CHAR, TYPE_DEFAULT, time_start, sizeof(time_start), &cb_time_start, DM_DESC_PTR_LEN_DEFAULT);
	dm_bind_column(hsmt, 2, DM_DATA_CHAR, TYPE_DEFAULT, time_end, sizeof(time_end), &cb_time_end, DM_DESC_PTR_LEN_DEFAULT);
	
	dm_describe_row_num(hsmt, &row_num);
		
	printf("plan_check_assign表机组数：%d\n", row_num);

	if(row_num == 0)
	{
		printf("机组%s在%s曲线考核第%d时段记录数为空！\n", sname, date, j);
		//write_program_action("错误","曲线考时段记录数为空！");
		return -2;
	}
	
	for (i = 0; i < row_num; ++i)
	{
		dm_fetch_scroll(hsmt, DM_FETCH_NEXT, 0);
		printf("time_start:%s\n", time_start);
		printf("time_end:%s\n", time_end);
		start_time = atoi(strtok(time_start, ":")) * 60 + atoi(strtok(NULL, ":")); 
		start_time = start_time / 15 - 1;
		end_time = atoi(strtok(time_end, ":")) * 60 + atoi(strtok(NULL, ":"));
		end_time = end_time / 15;
		printf("%d\n", start_time);
		printf("%d\n", end_time);
		if(start_time > end_time)
			continue;
		for(;start_time < end_time; start_time++)
		{
			if(strcmp(default_check_num, "1") == 0)
				unit_check_type[j][start_time] = 0;
			if(strcmp(default_check_num, "0") == 0)
				unit_check_type[j][start_time] = 1;
		}
	}

	return 0;
}

//获取考核标准
int get_check_rule()
{
	char check_rule[16];
	long cb_check_rule = 0;
	int i;
	
	char sqlstr[512];
	memset(sqlstr, 0, strlen(sqlstr));
	sprintf(sqlstr, "select svalue from plan.plan.sys_parameter_config where stype = '曲线考核' and sname = '考核标准'");  

	if (exec_sql(sqlstr) != 0)
	{
		printf("%s\n", "查询考核标准失败！");
		write_program_action("错误","查询考核标准失败！");
		return -1;
	}

	dm_bind_column(hsmt, 1, DM_DATA_CHAR, TYPE_DEFAULT, check_rule, sizeof(check_rule), &cb_check_rule, DM_DESC_PTR_LEN_DEFAULT);
	
	dm_describe_row_num(hsmt, &row_num);

	if(row_num == 0)
	{
		printf("%s\n", "没有定考核标准的记录!");
		write_program_action("错误","没有定考核标准的记录！");
		return -2;
	}

	for (i = 0; i < row_num; ++i)
	{
		dm_fetch_scroll(hsmt, DM_FETCH_NEXT, 0);
		sprintf(curve_check_rule, check_rule, 16);
	}

	printf("考核标准：%s\n", curve_check_rule);

	return 0;
}

//获取考核比例
int get_check_ratio()
{
	char check_ratio[16];
	long cb_check_ratio = 0;
	int i;
	
	char sqlstr[512];
	memset(sqlstr, 0, strlen(sqlstr));
	sprintf(sqlstr, "select svalue from plan.plan.sys_parameter_config where stype = '曲线考核' and sname = '火电考核比例'");  

	if (exec_sql(sqlstr) != 0)
	{
		printf("%s\n", "查询考核标准失败！");
		write_program_action("错误","查询考核标准失败！");
		return -1;
	}

	dm_bind_column(hsmt, 1, DM_DATA_CHAR, TYPE_DEFAULT, check_ratio, sizeof(check_ratio), &cb_check_ratio, DM_DESC_PTR_LEN_DEFAULT);
	
	dm_describe_row_num(hsmt, &row_num);

	if(row_num == 0)
	{
		printf("%s\n", "没有定考核标准的记录！");
		write_program_action("错误","没有定考核标准的记录！");
		return -2;
	}

	for (i = 0; i < row_num; ++i)
	{
		dm_fetch_scroll(hsmt, DM_FETCH_NEXT, 0);
		sprintf(curve_check_ratio, check_ratio, 16);
	}

	printf("考核比例：%s\n", curve_check_ratio);

	return 0;
}

//将时段转换成时间
void int_to_time(int d, char* timestr)
{
	int hour = floor(d / 4);
	char hourstr[2];
	if(hour < 10)
		sprintf(hourstr, "0%d", hour);
	else
		sprintf(hourstr, "%d", hour);

	int minute = d % 4;
	char minutestr[2];
	if(minute == 0)
		sprintf(minutestr, "00");
	else
		sprintf(minutestr, "%d", minute *15);
	
	sprintf(timestr, "%s:%s", hourstr, minutestr);
}

//获取机组实际出力
int get_unit_real_value(char* date, char* time, char* name, char* r_value)
{
	char real_value[16];
	long cb_real_value = 0;
	int i;
	
	char sqlstr[512];
	memset(sqlstr, 0, strlen(sqlstr));
	sprintf(sqlstr, "select real_value from plan.plan.unit_electric_real where sdate = '%s' and stime = '%s' and sname = '%s'", date, time, name);  

	if (exec_sql(sqlstr) != 0)
	{
		printf("%s在%s日%s的实际出力查询失败.\n", name, date, time);
		write_program_action("错误","查询实际出力失败！");
		return -1;
	}

	dm_bind_column(hsmt, 1, DM_DATA_CHAR, TYPE_DEFAULT, real_value, sizeof(real_value), &cb_real_value, DM_DESC_PTR_LEN_DEFAULT);
	
	dm_describe_row_num(hsmt, &row_num);

	if(row_num == 0)
	{
		printf("%s在%s日%s的实际出力值没有定义.\n", name, date, time);
		//write_program_action("错误","没有实际出力的记录！");
		return -2;
	}

	for (i = 0; i < row_num; ++i)
	{
		dm_fetch_scroll(hsmt, DM_FETCH_NEXT, 0);
		sprintf(r_value, real_value, 16);
	}

	printf("实际出力：%s\n", r_value);

	return 0;
}


//获取机组计划出力
int get_unit_plan_value(char* date, char* time, char* name, char* p_value)
{
	char plan_value[16];
	long cb_plan_value = 0;
	int i;
	
	char sqlstr[512];
	memset(sqlstr, 0, strlen(sqlstr));
	sprintf(sqlstr, "select plan_value from plan.plan.unit_electric_plan where sdate = '%s' and stime = '%s' and sname = '%s'", date, time, name);  

	if (exec_sql(sqlstr) != 0)
	{
		printf("%s在%s日%s的计划出力查询失败.\n", name, date, time);
		write_program_action("错误","查询计划出力失败！");
		return -1;
	}

	dm_bind_column(hsmt, 1, DM_DATA_CHAR, TYPE_DEFAULT, plan_value, sizeof(plan_value), &cb_plan_value, DM_DESC_PTR_LEN_DEFAULT);
	
	dm_describe_row_num(hsmt, &row_num);

	if(row_num == 0)
	{
		printf("%s在%s日%s的计划出力值没有定义.\n", name, date, time);
		//write_program_action("错误","没有计划出力的记录！");
		return -2;
	}

	for (i = 0; i < row_num; ++i)
	{
		dm_fetch_scroll(hsmt, DM_FETCH_NEXT, 0);
		sprintf(p_value, plan_value, 16);
	}

	printf("计划出力：%s\n", p_value);

	return 0;
}
