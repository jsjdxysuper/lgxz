#ifndef GLOBAL_VARIABLE_H
#define GLOBAL_VARIABLE_H

#include<stdio.h>

//机组信息
typedef struct 
{
	char status[512];	//机组状态
	char sdate[512];	//日期
	char stime[512];	//时间
	char sname[512];	//机组名
	char stype[512];	//考核类型
	float real_value;	//实际值
	float check_ratio;	//考核比例
	float plan_value;	//计划值
	float check_rule;	//考核标准
	float break_power;	//违约电力
	float break_ratio;	//违约比例
	float result_num;	//分数
}plan_check_results;

typedef struct
{
	char sname[512];	//机组名
}struct_unit_name;

char date[10];	//运行时，传人的参数，日期

char curve_check_ratio[16];	//违约比例
char curve_check_rule[16];	//违约标准
char default_check_num[16];	//默认考核类型

//const int SQL_MAX_LEN = 10000;	//sql??????

char current_date[10];	//程序运行日期
char current_time[10];	//程序运行时间


char program_id[20];	//程序id
char program_desc[100];	//程序描述
char program_type[100];	//程序类型

//struct_unit_name unit_name[512];	//默认有512个机组

#define TIME_SEPRATOR 96	//96个时段

//int unit_check_type[512][TIME_SEPRATOR];  //512个机组对应的96个时段数组

#endif
