curve_check: curve_check.o log.o dm_operater.o inifile.o
	gcc -o curve_check curve_check.o log.o dm_operater.o inifile.o -I /opt/dmdbms/bin/include/ -L /opt/dmdbms/bin/ -ldmapi
curve_check.o: curve_check.c log.h dm_operater.h global_variable.h
	gcc -c curve_check.c -I /opt/dmdbms/bin/include/
dm_operater.o: dm_operater.c log.h inifile.h global_variable.h
	gcc -c dm_operater.c -I /opt/dmdbms/bin/include/ 
log.o: log.c
	gcc -c log.c -I /opt/dmdbms/bin/include/ 
inifile.o: inifile.c
	gcc -c inifile.c -I /opt/dmdbms/bin/include/ 

clean:
	rm -rf *.o
