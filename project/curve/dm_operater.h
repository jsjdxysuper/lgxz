/***********************************************************
*note: dm database operater
*write by changxing, 2013.5
************************************************************/

#ifndef DM_OPERATOR_H
#define DM_OPERATOR_H

#include <stdio.h>  
#include <stdlib.h>  
#include "api.h"
#include <math.h>
#include "log.h"
#include "inifile.h"
#include "global_variable.h"

dm_henv henv;   //environment handle
dm_hdbc hdbc;   //connection handle
dm_hstmt hsmt;  // statement handle
dm_bool sret;   //return code
long long row_num;

char ipstr[MAXSIZE];
char usrstr[MAXSIZE];
char pwdstr[MAXSIZE];

#define INI_PATH "./db.ini"


int init_database();

int exec_sql(char *sql);  //execute insert procedure

int close_database();

#endif
