#include "log.h"

/*********************************************
* note: get local time
* @param[in]: buffer
* @return: none
**********************************************/
void get_local_time(char* buffer)
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
long get_file_size(char* filename)
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
void write_log_file(char* filename, long max_size, char* buffer, unsigned buf_size)
{
	if (filename != NULL && buffer != NULL)
	{
		// log size is biger than max_size, delete log file
		long length = get_file_size(filename);

		if (length > max_size)
		{
			unlink(filename); // 删除文件
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

