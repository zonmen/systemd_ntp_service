#!/bin/bash

#enable and start sync time service
count=0
systemctl enable systemd-timesyncd.service
#start sync time service
systemctl start systemd-timesyncd.service
#check if sync time service is active
IS_ACTIVE=$(systemctl is-active systemd-timesyncd.service)
if [ "$IS_ACTIVE" == "inactive" ]; then
   while [ $count -lt 5 ];
   do
      IS_ACTIVE=$(systemctl is-active systemd-timesyncd.service)
	echo 'try start srvice ...'
      let count=$count+1
   done
   if [ "$IS_ACTIVE" == "inactive" ]; then 
       echo 'error start time sync'
       exit 1
       fi
fi
sleep 0.1
#disable and stop time sync
systemctl disable systemd-timesyncd.service;
systemctl stop systemd-timesyncd.service;

#time-delay
let x=10
sleep $x

#compile c file
gcc /usr/local/bin/service/main.c -o /usr/local/bin/service/main;
#get difference
dif=$(/usr/local/bin/service/main)
#printf data to file "output.log"
echo $dif > /usr/local/bin/service/output.log
#delete main
rm /usr/local/bin/service/main






