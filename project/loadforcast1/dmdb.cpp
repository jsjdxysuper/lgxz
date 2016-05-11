#include<iostream>
#include"dmdb.h"
using std::cout;
using std::endl;

#define FILE_MAX_SIZE (1024*1024)	//file size
#define		 COLUMN_LEN		1024
#define MAX_BUFFER  2048
static char buffer[MAXSIZE];
static char mess_error[MAXSIZE];

dm_henv henv;   //environment handle
dm_hdbc hdbc;   //connection handle
dm_hstmt hsmt;  // statement handle
dm_bool sret;   //return code
/********************************************
* note: init database connection
* return: false: -1, -2,-3,-4; true: 0
********************************************/
int CDmDb::init_database()
{
    // get server ip, usr, pwd

    memset(buffer, 0 ,sizeof(buffer));
    sprintf(buffer, "%s\n", "init database connection");
    write_log_file(m_cLogPath, FILE_MAX_SIZE, buffer, strlen(buffer));

    //create API environment
    printf("create API environment\n");
    sret = dm_api_init();
    if(!(sret == DM_OK || sret == DM_SUCCESS_WITH_INFO))
    {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "create API environment failure!\n");
        write_log_file(m_cLogPath, FILE_MAX_SIZE, buffer, strlen(buffer));
        printf("%s",buffer);
        return -1;
    }

    // alloc environment handle
    printf("alloc enironment handle\n");
    if(!dm_alloc_env(&henv))
    {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "alloc environment handle failure!\n");
        write_log_file(m_cLogPath, FILE_MAX_SIZE, buffer, strlen(buffer));
        printf("%s",buffer);
        return -2;
    }

    //alloc connection handle
    printf("alloc connection handle\n");
    if(!dm_alloc_connect(henv, &hdbc))
    {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer,  "alloc connection handle failure!\n");
        write_log_file(m_cLogPath, FILE_MAX_SIZE, buffer, strlen(buffer));
        printf("%s",buffer);
        dm_free_env(henv);
        return -3;
    }

    //connect localhost
    printf("connect localhost\n");
cout<<m_cAddress<<m_cUserName<<m_cPassword<<endl;
    if(!dm_login_port(hdbc, (signed char*)m_cAddress,(signed char*)m_cUserName,
                      (signed char*)m_cPassword, m_nPort))
    {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "connect localhost failure!\n");
        write_log_file(m_cLogPath, FILE_MAX_SIZE, buffer, strlen(buffer));
        printf("%s",buffer);
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
int CDmDb::close_database()
{
    //free statement handle
    printf("free statement handle\n");
    if(!dm_free_stmt(hsmt))
    {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "free statement handle failure!\n");
        write_log_file(m_cLogPath, FILE_MAX_SIZE, buffer, strlen(buffer));
    }

    //disconnect DM
    printf("disconnect DM\n");
    if(!dm_logout(hdbc))
    {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "disconnect DM failure!\n");
        write_log_file(m_cLogPath, FILE_MAX_SIZE, buffer, strlen(buffer));
    }

    //free connection handle
    printf("free connection handle\n");
    if(!dm_free_connect(hdbc))
    {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "free connection handle failure!\n");
        write_log_file(m_cLogPath, FILE_MAX_SIZE, buffer, strlen(buffer));
    }

    //free environment handle
    printf("free environment handle\n");
    if(!dm_free_env(henv))
    {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "free environment handle failure!\n");
        write_log_file(m_cLogPath, FILE_MAX_SIZE, buffer, strlen(buffer));
    }
    return 0;
}


/********************************************************************
* note: exe sql
* @param: sql
* return: false -1, -2; true 0;
********************************************************************/
int CDmDb::exec_sql(char* sql)
{
    //alloc statement handle
    printf("alloc statement handle\n");
    if(!dm_alloc_stmt(hdbc, &hsmt))
    {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "alloc statement handle failure!\n");
        write_log_file(m_cLogPath, FILE_MAX_SIZE, buffer, strlen(buffer));
    //	printf(buffer);
        dm_logout(hdbc);
        dm_free_connect(hdbc);
        dm_free_env(henv);
        return -1;
    }

    //execute procedure
    printf("execute sql\n");
    printf("sql:%s\n", sql);
    if(!dm_direct_exec(hsmt, (signed char*)sql))
    {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "execute sql failure!\n");
        write_log_file(m_cLogPath, FILE_MAX_SIZE, buffer, strlen(buffer));
        return -2;
    }
    cout<<"hello"<<endl;
    return 0;
}

int CDmDb::PERROR_CLOSEDATABASE_EXIT(int ret,const char *str)
{
    sprintf(mess_error,"error in file:%s,line:%d,error:%s,",__FILE__,__LINE__, str);
   // fprintf(stderr,"%s", mess_error);
    close_database();
    fprintf(stderr, "%s\n", str);
    exit(ret);
}

string CDmDb::get_select_data(long long row, long column)
{
    return string(
                m_cDbData+row*m_nColumnNum*COLUMN_LEN+column*COLUMN_LEN
                );

}
/**
 * @brief exec_select sql sentence
 * @param sql_string: the sql sentence
 * @param column_num select: the num of data column
 * @param desc: select description
 * @param db_data selectִнڴ˽ṹ
 * @return ִexec_sql:return the row num what is got
 *
 */
long long CDmDb::exec_select(char *sql_string, int column_num, char *desc)
{
//===================================================================================
    long long i;
    int flag, j;

    char stru[10][COLUMN_LEN];
    int  str_size[10];
   // strcpy(sql_string_buff, sql_string);

    m_nColumnNum = column_num;
    char mess[MAX_BUFFER];
    cout<<"sql_string:"<<sql_string<<endl;
    sprintf(mess, "%s sql exect error", desc);
    if( exec_sql(sql_string) != 0 )
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    for(j=0; j<m_nColumnNum; j++)
    {
        memset(mess,'\0',sizeof(mess));
        sprintf(mess,"%s bound %d error", desc, j+1);
        if( DM_FALSE == dm_bind_column(hsmt, j+1, DM_DATA_CHAR, TYPE_DEFAULT,
                    stru[j], sizeof(stru[j]), &str_size[j],
                    DM_DESC_PTR_LEN_DEFAULT) )
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }

    sprintf(mess, "%s get row_num error", desc);
    if(DM_FALSE == dm_describe_row_num(hsmt, &m_nRowNum) )
         PERROR_CLOSEDATABASE_EXIT(-1, mess);

    sprintf(mess, "%s row num below zero", desc);
    if( 0 >=m_nRowNum )
         PERROR_CLOSEDATABASE_EXIT(-1, mess);

    if(NULL !=m_cDbData){free(m_cDbData);m_cDbData=NULL;}

    m_cDbData = (char*)malloc( m_nColumnNum*m_nRowNum*COLUMN_LEN );
    memset(m_cDbData, '\0', m_nColumnNum*m_nRowNum*COLUMN_LEN);
    printf("m_cDbData malloc%lld\n", m_nColumnNum*m_nRowNum*COLUMN_LEN);

    for(i=0; i<m_nRowNum; i++)
    {
        flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
        if(DM_FALSE == flag)
            PERROR_CLOSEDATABASE_EXIT(-1, "error in dm_fetch_scroll()");
        else if(DM_NO_DATA_FOUND == flag)
            PERROR_CLOSEDATABASE_EXIT(-1,"error in "
                    "dm_fetch_scroll()");
        for(j=0; j<m_nColumnNum; j++)
        {
            strcpy( (char*)(m_cDbData + i*m_nColumnNum*COLUMN_LEN + j*COLUMN_LEN), stru[j]);
        }

    }
    return m_nRowNum;
}

/*********************************************
* note: get local time
* @param[in]: buffer
* @return: none
**********************************************/
void CDmDb::get_local_time(char* buffer)
{
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
        (timeinfo->tm_year+1900), timeinfo->tm_mon, timeinfo->tm_mday,
        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

/*********************************************
* note: get file size
* @param filename[in]: file path
* @return: file size
**********************************************/
long CDmDb::get_file_size(const char* filename)
{
    long length = 0;
    FILE *fp = NULL;

    fp = fopen(filename, "rb");
    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
    }

    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }

    return length;
}

/*********************************************************
* note: write log file
* @param filename[in]: log path
* @param maxsize[in]: log max size
* @param buffer[in]: log content
* @param buf_size[in]: log content size
*********************************************************/
void CDmDb::write_log_file(const char* filename, long max_size, char* buffer, unsigned buf_size)
{
    if (filename != NULL && buffer != NULL)
    {
        // log size is biger than max_size, delete log file
        long length = get_file_size(filename);

        if (length > max_size)
        {
            unlink(filename); // ɾļ
        }

        // write buffer in log
        {
            FILE *fp;
            fp = fopen(filename, "at+");
            if (fp != NULL)
            {
                char now[32];
                memset(now, 0, sizeof(now));
                get_local_time(now);
                sprintf(now, "%s\n", now);
                fwrite(now, strlen(now)+1, 1, fp);
                fwrite(buffer, buf_size, 1, fp);

                fclose(fp);
                fp = NULL;
            }
        }
    }
}
