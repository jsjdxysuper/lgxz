cal_run_check_cost_collect: cal_run_check_cost_collect.o log.o dm_operater.o inifile.o common.o 
	gcc -o cal_run_check_cost_collect cal_run_check_cost_collect.o log.o dm_operater.o inifile.o common.o -I /opt/dmdbms/bin/include/ -L /opt/dmdbms/bin/ -ldmapi
cal_run_check_cost_collect.o: cal_run_check_cost_collect.c log.h dm_operater.h cal_run_check_cost_collect.h
	gcc -c cal_run_check_cost_collect.c -I /opt/dmdbms/bin/include/
common.o: common.c common.h
	gcc -c common.c -o common.o
dm_operater.o: dm_operater.c log.h inifile.h
	gcc -c dm_operater.c -I /opt/dmdbms/bin/include/ 
inifile.o:inifile.c inifile.h
	gcc -c inifile.c -o inifile.o
log.o: log.c
	gcc -c log.c -I /opt/dmdbms/bin/include/ 

clean:
	rm -rf *.o
