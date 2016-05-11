/***********************************************************
*note: dm database operater
*write by changxing, 2013.5
************************************************************/

#ifndef DM_OPERATOR_H
#define DM_OPERATOR_H

#include <stdio.h>  
#include <stdlib.h>  
#include "api.h"
#include "log.h"

dm_henv henv;   //environment handle
dm_hdbc hdbc;   //connection handle
dm_hstmt hsmt;  // statement handle
dm_bool sret;   //return code

int init_database();

int exec_sql(char *sql);  //execute insert procedure

int close_database();

#endif
