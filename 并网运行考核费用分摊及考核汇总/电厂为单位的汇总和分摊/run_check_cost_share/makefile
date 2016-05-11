cal_run_check_cost_share: cal_run_check_cost_share.o log.o dm_operater.o common.o 
	gcc -o cal_run_check_cost_share cal_run_check_cost_share.o log.o dm_operater.o common.o -I /opt/dmdbms/bin/include/ -L /opt/dmdbms/bin/ -ldmapi
cal_run_check_cost_share.o: cal_run_check_cost_share.c log.h dm_operater.h cal_run_check_cost_share.h
	gcc -c -g cal_run_check_cost_share.c -I /opt/dmdbms/bin/include/
common.o: common.c common.h
	gcc -c -g common.c -o common.o
dm_operater.o: dm_operater.c log.h inifile.h
	gcc -c -g dm_operater.c -I /opt/dmdbms/bin/include/ 
inifile.o:inifile.c inifile.h
	gcc -c -g inifile.c -o inifile.o
log.o: log.c
	gcc -c -g log.c -I /opt/dmdbms/bin/include/ 

clean:
	rm -rf *.o
