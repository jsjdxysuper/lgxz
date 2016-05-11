#include<stdio.h>
#include"log.h"
#include"dm_operater.h"
int main()
{
	if(init_database()!=0)
		exit(1);
	//exec_sql("select *from plan.plan.run_check_cost_share");
	exec_sql("insert into plan.plan.run_check_cost_share (sdate,sname,month_electricity)values('2013-05-05','shenyangdianchang','555')");

}
