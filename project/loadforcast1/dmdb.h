#ifndef __DMDB_H__
#define __DMDB_H__

#include<string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include"api.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>       // linux
#endif
#include"basedb.h"

using std::string;

#define NAMEPASSLEN 30
#define MAXSIZE 512		//buffer length



class CDmDb
{
public:
    CDmDb(const char *user_name,const char *password,
          const char *address,
          const char *log_path, long port = 12345l):
        m_nPort(port),m_cDbData(NULL)
    {
        strcpy(m_cAddress,address);
        strcpy(m_cUserName,user_name);
        strcpy(m_cPassword,password);
        strcpy(m_cLogPath,log_path);
        int i=0;
        i++;
    }

    int init_database();

    int exec_sql(char *sql);  //execute insert procedure

    long long exec_select(char *sql_string, int column_num, char *desc);

    int PERROR_CLOSEDATABASE_EXIT(int ret,const char *str);

    string get_select_data(long long row,long colum);

    int close_database();

    void get_local_time(char* buffer);

    long get_file_size(const char* filename);

    void write_log_file(const char* filename, long max_size, char* buffer, unsigned buf_size);

    long long get_row_num(){return m_nRowNum;}

    ~CDmDb()
    {

        close_database();
        free(m_cDbData);
    }
private:

    //Data used to connect database.
    char m_cAddress[NAMEPASSLEN];
    char m_cDbName[NAMEPASSLEN];
    char m_cMode[NAMEPASSLEN];
    char m_cUserName[NAMEPASSLEN];
    char m_cPassword[NAMEPASSLEN];
    long   m_nPort;

    //Data used to write log file.
    char m_cLogPath[MAXSIZE];

    //Data used to get DataBase Data.
    char *m_cDbData;

    long long m_nRowNum;
    int m_nColumnNum;



};
#endif // DMDB_H
