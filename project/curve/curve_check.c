#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "log.h"
#include "dm_operater.h"
#include "global_variable.h"

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("please input a comand like \"./curve_check 2013-05-20\"\n");
		return 0;
	}

	int i, j, k;

	strcpy(date, argv[1]);

	memset(current_date, 0, sizeof(current_date));
	memset(current_time, 0, sizeof(current_time));

	get_date_time(current_date, current_time);

	printf("��ǰ���ڣ�%s\n��ǰʱ�䣺%s\n", current_date, current_time);

	sprintf(program_id, "curve_check");
	sprintf(program_desc, "���߿���");
	sprintf(program_type,"���߿���");
	
	//init database
	if (init_database() != 0)
	{
		printf("%s\n", "init database failure!");
		return -1;	
	}
	//���ϵͳĬ�ϵĿ�������
	if(get_check_num() != 0)
	{
		printf("��ȡϵͳĬ�Ͽ�������ʧ�ܣ�\n");
		close_database();
		return 0;
	}

	//��ÿ��˱�׼
	if(get_check_rule() != 0)
	{
		printf("��ȡϵͳĬ�Ͽ�������ʧ�ܣ�\n");
		close_database();
		return 0;
	}

	//��ÿ��˱���
	if(get_check_ratio() != 0)
	{
		printf("��ȡ���߿��˿��˱���ʧ�ܣ�\n");
		close_database();
		return 0;
	}

	int unit_name_count;
	struct_unit_name *unit_name;
	//���Ҫ�����߿��˵Ļ�����
	if((unit_name = get_curve_check_sname(date, &unit_name_count)) == NULL)
	{
		printf("%s�����߿��˻�������ѯʧ�ܣ�\n", date);
		close_database();
		return 0;
	}

	printf("\n----------------------------������Ϣ--------------------------------------\n");
	for(i = 0; i < unit_name_count; i++)
	{
		printf("��%d������Ϊ:%s\n", i + 1, unit_name[i].sname);
	}

	int **unit_check_type;
	unit_check_type = (int **)malloc(sizeof(int *) * unit_name_count);
	for(i = 0; i < unit_name_count; i++)
	{
		unit_check_type[i] = (int *)malloc(sizeof(int) * TIME_SEPRATOR);
	}

	//����96��ʱ�Σ�ȫ��ΪĬ�Ͽ�������
	printf("\n----------------------------������Ĭ�Ͽ�������--------------------------------------\n");
	for(i = 0; i < unit_name_count; i++)
	{
		printf("\n��%d������%sĬ�Ͽ�������\n", i + 1, unit_name[i].sname);
		for(j = 0; j < TIME_SEPRATOR; j++)
		{
			unit_check_type[i][j] = atoi(default_check_num);
			printf("%d:%d ", j + 1, unit_check_type[i][j]);
		}
	}
	
	
	//����plan_check_assign�Ŀ������ͣ��޸�unit_check_type�еĿ�������
	printf("\n----------------------------�������޸ĺ�Ŀ�������--------------------------------------\n");
	for (i = 0; i < unit_name_count; i++)
	{
		printf("��%d������%s��ʱ�ο�������\n", i + 1, unit_name[i].sname);
		if(get_all_time_check_type(unit_check_type, date, unit_name[i].sname, i) == -1)
		{
			close_database();
			return 0;
		}
		for(j = 0; j <  TIME_SEPRATOR; j++)
			printf("%d:%d ", j + 1, unit_check_type[i][j]);
	}
	
	
	//ʵ�ʳ������ƻ�������ֵ��plan_check_results
	plan_check_results *res = (plan_check_results *)malloc(sizeof(plan_check_results) * unit_name_count * TIME_SEPRATOR);
	if(res == NULL)
	{
           printf("plan_check_results init failed\n");
           close_database();
           return 0;
    }
	char timestr[5];
	char plan_value[16];
	char real_value[16];
	int curve_unit_name_count;

	printf("\n------------------------------�������ʱ����Ϣ----------------------------------------\n");
	for ( i = 0; i < unit_name_count; i++ )
	{
		printf("��%d̨�����ʱ����Ϣ\n", i + 1);
		for (j = 0; j < TIME_SEPRATOR; j++ )
		{
			k = i * TIME_SEPRATOR + j;
			sprintf(res[k].status, "����");
			sprintf(res[k].sdate, date);
			int_to_time(j+1, timestr);
			sprintf(res[k].stime, timestr);
			sprintf(res[k].sname, unit_name[i].sname);
			if(unit_check_type[i][j] == 1)
				sprintf(res[k].stype, "����");
			if(unit_check_type[i][j] == 0)
				sprintf(res[k].stype, "�⿼��");
			memset(plan_value, 0, sizeof(plan_value));
			if(get_unit_plan_value(date, timestr, unit_name[i].sname, plan_value) != 0 || strcmp(plan_value, ".000") == 0)
			{
				sprintf(res[k].status, "�ƻ���������");
				continue;
			}
			res[k].plan_value = atof(plan_value);
			res[k].check_ratio =  atof(curve_check_ratio);
			memset(real_value, 0, sizeof(real_value));
			if(get_unit_real_value(date, timestr, unit_name[i].sname, real_value) != 0 || strcmp(real_value, ".000") == 0)
			{
				sprintf(res[k].status, "ʵ�ʳ�������");
				continue;
			}
			res[k].real_value = atof(real_value);
			res[k].check_rule = atof(curve_check_rule);
			printf("%s : %s,%s,%s,%s,%s,%0.3f,%0.3f,%0.3f,%0.3f\n", res[k].stime, res[k].status, res[k].sdate, res[k].stime, res[k].sname, res[k].stype, res[k].real_value,res[k].check_ratio, res[k].plan_value, res[k].check_rule);
		}
	}
	free(unit_name);
	for(i=0;i<unit_name_count;i++)
	free(unit_check_type[i]);
	printf("\n----------------------------------caculate-------------------------------------\n");

	char deletestr[512];
	memset(deletestr, 0, sizeof(deletestr));
	char sqlstr[80000];
	memset(sqlstr, 0, sizeof(sqlstr));
	int plan_count = 0;
	int real_count = 0;
	//int temp = 0;
	char *program_correct_info = (char *)malloc(unit_name_count * (512 + 2 + 512) + 100);
	char *p = program_correct_info;
	memcpy(p, "������ȷִ��", strlen("������ȷִ��")); 
	p += strlen("������ȷִ��");
	char temp[1026];
	//char program_correct_info[20480]; //����ִ����Ϣ
	//char program_plan_electric_error[10240]; //����ƻ�ֵ������Ϣ
	//char program_real_electric_error[10240]; //����ʵ��ֵ������Ϣ
	//memset(program_plan_electric_error, 0, strlen(program_plan_electric_error));
	//memset(program_real_electric_error, 0, strlen(program_real_electric_error));
	//char plan_error[1024], real_error[1024];
	//memset(plan_error, 0, strlen(plan_error));
	//memset(real_error, 0, strlen(real_error));


	for( i = 0; i < unit_name_count; i++)
	{
		memset(sqlstr, 0, sizeof(sqlstr));
		for( j = 0; j < TIME_SEPRATOR; j++)
		{
			k = i * TIME_SEPRATOR + j;
			printf("%d status = %s\n", i + 1, res[k].status);
			if(strcmp(res[k].status, "�ƻ���������") == 0)
			{
				memset(temp, 0, strlen(temp));
				sprintf(temp, " %s%s ", res[k].sname, res[k].status);
				memcpy(p, temp, strlen(temp));
				p += strlen(temp);
				//memset(program_plan_electric_error, 0, strlen(program_plan_electric_error));
				//sprintf(program_plan_electric_error, "%s,%s", program_plan_electric_error, unit_name[i].sname);
				//strncat(plan_error, program_plan_electric_error, strlen(program_plan_electric_error) + 1);
				//printf("%d program_plan_electric_error %s\n", i + 1, program_plan_electric_error);
				break;
			}
			if(strcmp(res[k].status, "ʵ�ʳ�������") == 0)
			{	
				memset(temp, 0, strlen(temp));
				sprintf(temp, " %s%s ", res[k].sname, res[k].status);
				memcpy(p, temp, strlen(temp));
				p += strlen(temp);
				//memset(program_real_electric_error, 0, strlen(program_real_electric_error));
				//sprintf(program_real_electric_error, "%s,%s", program_real_electric_error, unit_name[i].sname);
				//strncat(real_error, program_real_electric_error, strlen(program_real_electric_error) + 1);
				//printf("%d program_real_electric_error %s \n", i + 1, program_real_electric_error);
				break;
			}
			if(strcmp(res[k].status, "����") == 0)
			{
				if(strcmp(res[k].stype, "����") == 0)
				{
					if((res[k].real_value - res[k].plan_value) > res[k].plan_value * res[k].check_ratio / 100)
					{
						res[k].break_power = res[k].real_value - res[k].plan_value - res[k].plan_value * res[k].check_ratio / 100;
						res[k].break_ratio = res[k].break_power / res[k].plan_value - res[k].check_ratio / 100;
						res[k].result_num = res[k].break_power / 40 * res[k].check_rule;
					}
					else if((res[k].plan_value - res[k].real_value) > res[k].plan_value * res[k].check_ratio / 100)
					{
						res[k].break_power = res[k].real_value - (res[k].plan_value - res[k].plan_value * res[k].check_ratio / 100);
						res[k].break_ratio = ((res[k].plan_value - res[k].plan_value * res[k].check_ratio / 100) - res[k].real_value) / res[k].plan_value - res[k].check_ratio / 100;
						res[k].result_num = ((res[k].plan_value - res[k].plan_value * res[k].check_ratio / 100) - res[k].real_value) / 40 * res[k].check_rule;	
					}
					else
					{
						res[k].break_power = 0;
						res[k].break_ratio = 0;
						res[k].result_num = 0;
					}
				}
				if(strcmp(res[k].stype, "�⿼��") == 0)
				{
					res[k].break_power = 0;
					res[k].break_ratio = 0;
					res[k].result_num = 0;
				}

				if(j == 0)
					sprintf(sqlstr, "insert into plan.plan.plan_check_result "
							"select '%s','%s','%s','%s','%0.3f','%0.3f','%0.3f','%0.3f','%0.3f','%0.3f','%0.3f' ",
							res[k].sdate, res[k].stime, res[k].sname, res[k].stype, res[k].real_value,res[k].check_ratio, res[k].plan_value, res[k].check_rule,
							res[k].break_power, res[k].break_ratio, res[k].result_num);
				else
					sprintf(sqlstr, "%s "
							"union all "
							"select '%s','%s','%s','%s','%0.3f','%0.3f','%0.3f','%0.3f','%0.3f','%0.3f','%0.3f' ",
							sqlstr,
							res[k].sdate, res[k].stime, res[k].sname, res[k].stype, res[k].real_value,res[k].check_ratio, res[k].plan_value, res[k].check_rule,
							res[k].break_power, res[k].break_ratio, res[k].result_num);
				//printf("��ǰ��%d������ű�%s\n", i, sqlstr);
				printf("sqlstr��С:%d\n", strlen(sqlstr));


				if(j == TIME_SEPRATOR - 1)
				{
					//memset(deletestr, 0, sizeof(deletestr));
					sprintf(deletestr, "delete plan.plan.plan_check_result where sdate = '%s' and sname = '%s'", 
						res[k].sdate, res[k].sname);
					if (exec_sql(deletestr) != 0)
					{
						write_program_action("����","ɾ������ʧ�ܣ�");			
						close_database();
						return 0;
					}
					if (exec_sql(sqlstr) != 0)
					{
						write_program_action("����","��������ʧ�ܣ�");			
						close_database();
						return 0;
					}
				}

				printf("��%d̨����%dʱ����Ϣ:'%s','%s','%s','%s','%0.3f','%0.3f','%0.3f','%0.3f','%0.3f','%0.3f','%0.3f'\n", 
					i, j,
					res[k].sdate, res[k].stime, res[k].sname, res[k].stype, res[k].real_value,res[k].check_ratio, res[k].plan_value, res[k].check_rule, 
					res[k].break_power, res[k].break_ratio, res[k].result_num);
			}
		}
	}
	free(res);
	
	/*memset(program_correct_info, 0, strlen(program_correct_info));
	sprintf(program_correct_info, "������ȷִ��");
	if(strcmp(program_plan_electric_error, "") != 0)
		sprintf(program_correct_info, "%s%s%s�ƻ�ֵ����", program_correct_info, program_plan_electric_error, date);
	if(strcmp(program_real_electric_error, "") != 0)
		sprintf(program_correct_info, "%s%s%sʵ��ֵ����", program_correct_info, program_real_electric_error, date);
	printf("%d\n",strlen(program_correct_info));*/
	write_program_action("��ȷ",program_correct_info);
	close_database();

	return 0;
}
