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

char cost_collect_order[][30]=
{
	{"AGC服务"},//0
	{"安全管理"},//1
	{"电厂高压侧电气设备"},//2
	{"调度业务管理"},//3
	{"调峰服务"},//4
	{"非计划停机考核"},//5
	{"风电预测考核"},//6
	{"黑启动服务"},//7
	{"机组检修"},//8
	{"继电保护"},//9
	{"励磁和PSS装置管理"},//10
	{"曲线违约考核"},//11
	{"水库调度"},//12
	{"通信"},//13
	{"无功调节服务"},//14
	{"一次调频服务"},//15
	{"自动化"}//16
};

#define COLLECT_NUM		17//考核汇总项目数量
typedef struct
{
	char type[20];
	char month[20];
}para_stru;
para_stru para;

typedef struct 
{
	char plant_name[256];
	double value[17];
}cost_collect_stru;




int get_unit_info(cost_collect_stru ** cost_collect17, char *month, long long* plant_num);
int get_cost_result(char *month, cost_collect_stru **cost_collect17, long long* plant_num);
int write_share_stru(char*month,cost_collect_stru **cost_collect17, long long* plant_num);
int calculate_run_check_cost_share();
int write_act_info(char *sresult,char *note);

#endif

