#include "dm_operater.h"
#include"api.h"

/********************************************
* note: init database connection
* return: false: -1, -2,-3,-4; true: 0
********************************************/
int init_database()
{
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
	if(!dm_login_port(hdbc, "localhost", "SYSDBA", "SYSDBA", 12345))
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
	printf("alloc statement handle\n");
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
	printf("execute procedure\n");
	printf("procedure:%s\n", sql);
	if(!dm_direct_exec(hsmt, sql))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "execute procedure failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		return -2;
	}	
	return 0;		
}
