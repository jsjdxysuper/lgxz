/*******************************************************************************
 * class for reading Ini File on Unix/Linux/Window
*******************************************************************************/
#ifndef __CINIFILE_H__
#define __CINIFILE_H__

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

//#include <unistd.h>
#ifdef WIN32  
#include <windows.h>  
#else  
#include <unistd.h>       // linux
#endif

//mode == 0 Ö»¶Á  mode > 0 ¶ÁÐ´
void    *IniOpenFile(const char * psz_file, const unsigned char mode);
int     IniCleanFile(void *fpini);
int     IniCloseFile(void *fpini);
int     IniFileSizeGet(void *fpini);

int     IniGetString(const char * psz_section, const char * psz_key, char * psz_value, int value_size, void *fpini);
int     IniSetString(const char * psz_section, const char * psz_key, const char * psz_value, void *fpini);

int     IniGetInteger(const char * psz_section, const char * psz_key, int i_default, void *fpini);
int     IniSetInteger(const char * psz_section, const char * psz_key, const int i_value, void *fpini);

long    IniGetLong(const char * psz_section, const char * psz_key, long i_default, void *fpini);
int     IniSetLong(const char * psz_section, const char * psz_key, const long i_value, void *fpini);

double  IniGetDouble(const char * psz_section, const char * psz_key, double i_default, void *fpini);
int     IniSetDouble(const char * psz_section, const char * psz_key, const double i_value, void *fpini);

int     IniGetBool(const char * psz_section, const char * psz_key, int b_default, void *fpini);
int     IniSetBool(const char * psz_section, const char * psz_key, const int b_value, void *fpini);

#endif // __CINIFILE_H__


