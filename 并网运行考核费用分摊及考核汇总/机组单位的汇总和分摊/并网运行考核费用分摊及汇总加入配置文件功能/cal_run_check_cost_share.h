#ifndef CAL_RUN_CHECK_COST_SHARE_H
#define CAL_RUN_CHECK_COST_SHARE_H

//考分分值对应人民币的数目
#define COST_PER_MARK 1000	
#define  PERROR_CLOSEDATABASE_EXIT_IF(dec,ret,str) if((dec)==DM_FALSE)\
        {fprintf(stderr,"error in: %s,file:%s,line:%d\n",str,__FILE__,__LINE__);close_database();exit(ret);}
		
#define  PERROR_CLOSEDATABASE_EXIT(ret,str)\
		{fprintf(stderr,"error in: %s,file:%s,line:%d\n",str,__FILE__,__LINE__);write_act_info("失败",str);close_database();exit(ret);}


//与运行考核费用分摊表run_check_cost_share一致的数据结构
typedef struct
{
	char   year_month[10];     //年月
	char   plant_name[100];	   //电厂名称
	double month_electricity;  //月度上网电量
	double check_cost;		   //考核费用
	double share_cost;		   //分摊费用
	double account_cost;	   //结算费用
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
	/*double agc_service; 		 //AGC服务1
	double security_manage;		 //安全管理2
	double high_voltage_device;	 //电厂高压侧电器设备3
	double dispatch_manage;		 //调度业务管理4
	double peak_load_regulation; //调峰服务5
	double unplan_shutdown_check;//非计划停机考核6
	double wind_power_forcast;	 //风电预测考核7
	double black_start_service;	 //黑启动服务8
	double unit_maintenance;	 //机组检修9
	double relay_protection;	 //继电保护10
	double excitation_pss_manage;//励磁和pss装置管理11
	double curve_default_check; //曲线违约考核12
	double reservoir_operation;	//水库调度13
	double communication;		//通信14
	double reactive_regulation_service;//无功调节服务15
	double onetime_frequency_modulation;//一次调频服务16
	double automation;			//自动化17*/
}cost_collect_stru;




int get_unit_info(cost_share_stru **run_check_cost_share, cost_collect_stru ** cost_collect17, char *month, long long* plant_num);
int get_cost_result(char *month,cost_share_stru **cost_share_array, cost_collect_stru **cost_collect17, long long* plant_num);
int get_month_electricity(char *month,cost_share_stru **cost_share_array,long long* plant_num);
int write_share_stru(char*month,cost_share_stru **cost_share_array, cost_collect_stru **cost_collect17, long long* plant_num);
int calculate_run_check_cost_share();
int write_act_info(char *sresult,char *note);

#endif

