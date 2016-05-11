#include "dm_operater.h"
#include "inifile.h"

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
	//	printf(buffer);
		dm_logout(hdbc);
		dm_free_connect(hdbc);
		dm_free_env(henv);
		return -1;
	}

	//execute procedure
	//printf("execute sql\n");
	//printf("sql:%s\n", sql);
	if(!dm_direct_exec(hsmt, sql))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "execute sql failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		return -2;
	}	
	return 0;		
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
	
}

