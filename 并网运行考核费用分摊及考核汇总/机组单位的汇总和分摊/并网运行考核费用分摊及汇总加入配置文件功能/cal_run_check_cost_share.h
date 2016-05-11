#ifndef CAL_RUN_CHECK_COST_SHARE_H
#define CAL_RUN_CHECK_COST_SHARE_H

//���ַ�ֵ��Ӧ����ҵ���Ŀ
#define COST_PER_MARK 1000	
#define  PERROR_CLOSEDATABASE_EXIT_IF(dec,ret,str) if((dec)==DM_FALSE)\
        {fprintf(stderr,"error in: %s,file:%s,line:%d\n",str,__FILE__,__LINE__);close_database();exit(ret);}
		
#define  PERROR_CLOSEDATABASE_EXIT(ret,str)\
		{fprintf(stderr,"error in: %s,file:%s,line:%d\n",str,__FILE__,__LINE__);write_act_info("ʧ��",str);close_database();exit(ret);}


//�����п��˷��÷�̯��run_check_cost_shareһ�µ����ݽṹ
typedef struct
{
	char   year_month[10];     //����
	char   plant_name[100];	   //�糧����
	double month_electricity;  //�¶���������
	double check_cost;		   //���˷���
	double share_cost;		   //��̯����
	double account_cost;	   //�������
}cost_share_stru;

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

typedef struct 
{
	char plant_name[256];
	double value[17];
	/*double agc_service; 		 //AGC����1
	double security_manage;		 //��ȫ����2
	double high_voltage_device;	 //�糧��ѹ������豸3
	double dispatch_manage;		 //����ҵ�����4
	double peak_load_regulation; //�������5
	double unplan_shutdown_check;//�Ǽƻ�ͣ������6
	double wind_power_forcast;	 //���Ԥ�⿼��7
	double black_start_service;	 //����������8
	double unit_maintenance;	 //�������9
	double relay_protection;	 //�̵籣��10
	double excitation_pss_manage;//���ź�pssװ�ù���11
	double curve_default_check; //����ΥԼ����12
	double reservoir_operation;	//ˮ�����13
	double communication;		//ͨ��14
	double reactive_regulation_service;//�޹����ڷ���15
	double onetime_frequency_modulation;//һ�ε�Ƶ����16
	double automation;			//�Զ���17*/
}cost_collect_stru;




int get_unit_info(cost_share_stru **run_check_cost_share, cost_collect_stru ** cost_collect17, char *month, long long* plant_num);
int get_cost_result(char *month,cost_share_stru **cost_share_array, cost_collect_stru **cost_collect17, long long* plant_num);
int get_month_electricity(char *month,cost_share_stru **cost_share_array,long long* plant_num);
int write_share_stru(char*month,cost_share_stru **cost_share_array, cost_collect_stru **cost_collect17, long long* plant_num);
int calculate_run_check_cost_share();
int write_act_info(char *sresult,char *note);

#endif

