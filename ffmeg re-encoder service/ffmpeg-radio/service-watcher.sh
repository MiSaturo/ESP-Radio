#!/bin/sh
serviceUrl=http://127.0.0.1:8000/payam

parentPid=$1
echo "parent pid=$parentPid"
while :
do
	echo "Checking... (Press [CTRL+C] to stop)"
	sleep 2
	status_code=$( curl -sIXGET  --write-out '%{http_code}' --silent --output /dev/null $serviceUrl)
	
	if [ "$status_code" -ne 200 ] ; then
	   echo "got status of $status_code"
	   echo "force exiting..."
	   # service ffmpeg-radio restart 
	   # kill -9 $parentPid
	   killall -9 icecast2 2>/dev/null
	   pkill -9 -P $parentPid
	   break
	fi
done



