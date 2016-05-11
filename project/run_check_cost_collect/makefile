DM_PATH=/home/d5000/dalian/dmdbms
cal_run_check_cost_collect: cal_run_check_cost_collect.o log.o dm_operater.o inifile.o common.o 
	gcc -o cal_run_check_cost_collect cal_run_check_cost_collect.o log.o dm_operater.o inifile.o common.o -I $(DM_PATH)/bin/include/ -L $(DM_PATH)/bin/ -ldmapi
cal_run_check_cost_collect.o: cal_run_check_cost_collect.c log.h dm_operater.h cal_run_check_cost_collect.h
	gcc -c -g cal_run_check_cost_collect.c -I $(DM_PATH)/bin/include/
common.o: common.c common.h
	gcc -c -g common.c -o common.o -I $(DM_PATH)/bin/include/
dm_operater.o: dm_operater.c log.h inifile.h
	gcc -c -g dm_operater.c -I $(DM_PATH)/bin/include/ 
inifile.o:inifile.c inifile.h
	gcc -c -g inifile.c -o inifile.o -I $(DM_PATH)/bin/include/
log.o: log.c
	gcc -c -g log.c -I $(DM_PATH)/bin/include/ 

clean:
	rm -rf *.o
