/***********************************************************
* note: define log file
* write by changxing, 2013.5
************************************************************/
#ifndef LOG_H
#define LOG_H

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <time.h>  
#ifdef WIN32  
#include <windows.h>  
#else  
#include <unistd.h>       // linux
#endif

#define LOGPATH "/home/log" //log path
#define MAXSIZE 1024		//buffer length
#define FILE_MAX_SIZE (1024*1024)	//file size

char buffer[MAXSIZE];	//

void get_local_time(char* buffer);

long get_file_size(char* filename);

void write_log_file(char* filename, long max_size, char* buffer, unsigned buf_size);
#endif
