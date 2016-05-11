/***********************************************************
*note: dm database operater
*write by changxing, 2013.5
************************************************************/

#ifndef DM_OPERATOR_H
#define DM_OPERATOR_H

#include <stdio.h>  
#include <stdlib.h>  
#include <api.h>
#include "log.h"
#include "inifile.h"

dm_henv henv;   //environment handle
dm_hdbc hdbc;   //connection handle
dm_hstmt hsmt;  // statement handle
dm_bool sret;   //return code

typedef struct
{
	char sdate[30];
	char stime[30];
	char stype[50];
	char sid[300];
	char sdesc[200];
	char sresult[200];
	char note[500];
}act_info_stru;


long long row_num;

char ipstr[MAXSIZE];
char usrstr[MAXSIZE];
char pwdstr[MAXSIZE];

act_info_stru act_info_to_write;//程序执行信息

#define INI_PATH "./db.ini"


int init_database();

int exec_sql(char *sql);  //execute insert procedure

int close_database();

int write_act_info(char *sresult,char *note);

#endif
