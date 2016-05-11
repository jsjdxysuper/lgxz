#ifndef GLOBAL_VARIABLE_H
#define GLOBAL_VARIABLE_H

#include<stdio.h>

//������Ϣ
typedef struct 
{
	char status[512];	//����״̬
	char sdate[512];	//����
	char stime[512];	//ʱ��
	char sname[512];	//������
	char stype[512];	//��������
	float real_value;	//ʵ��ֵ
	float check_ratio;	//���˱���
	float plan_value;	//�ƻ�ֵ
	float check_rule;	//���˱�׼
	float break_power;	//ΥԼ����
	float break_ratio;	//ΥԼ����
	float result_num;	//����
}plan_check_results;

typedef struct
{
	char sname[512];	//������
}struct_unit_name;

char date[10];	//����ʱ�����˵Ĳ���������

char curve_check_ratio[16];	//ΥԼ����
char curve_check_rule[16];	//ΥԼ��׼
char default_check_num[16];	//Ĭ�Ͽ�������

//const int SQL_MAX_LEN = 10000;	//sql??????

char current_date[10];	//������������
char current_time[10];	//��������ʱ��


char program_id[20];	//����id
char program_desc[100];	//��������
char program_type[100];	//��������

//struct_unit_name unit_name[512];	//Ĭ����512������

#define TIME_SEPRATOR 96	//96��ʱ��

//int unit_check_type[512][TIME_SEPRATOR];  //512�������Ӧ��96��ʱ������

#endif
