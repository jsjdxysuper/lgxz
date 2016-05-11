cal_run_check_cost_share: cal_run_check_cost_share.o log.o dm_operater.o 
	gcc -o cal_run_check_cost_share cal_run_check_cost_share.o log.o dm_operater.o -I /home/d5000/dalian/dmdbms/bin/include/ -L /home/d5000/dalian/dmdbms/bin/ -ldmapi
cal_run_check_cost_share.o: cal_run_check_cost_share.c log.h dm_operater.h cal_run_check_cost_share.h
	gcc -c cal_run_check_cost_share.c -I /home/d5000/dalian/dmdbms/bin/include/
dm_operater.o: dm_operater.c log.h
	gcc -c dm_operater.c -I /home/d5000/dalian/dmdbms/bin/include/ 
log.o: log.c
	gcc -c log.c -I /home/d5000/dalian/dmdbms/bin/include/ 

clean:
	rm -rf *.o
