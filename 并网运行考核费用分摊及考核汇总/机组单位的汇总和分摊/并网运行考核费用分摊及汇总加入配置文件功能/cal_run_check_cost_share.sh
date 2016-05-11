#!/bin/sh
if [ $# != 1 ]; then
   echo "please input a parameter,such as 2013-05!"
   exit -1;
fi
cd /home/d5000/dalian/xuyang/project/dm
./cal_run_check_cost_share $1
