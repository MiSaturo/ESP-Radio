#!/bin/sh

cd "$(dirname "$0")"

pid=$$

killall -9 icecast2 2>/dev/null

mkdir -p /var/log/icecast2
chown icecast2:icecast /var/log/icecast2
rm /var/log/icecast2/*

chown icecast2:icecast icecast.xml

sudo -u icecast2 icecast2 -c ./icecast.xml&
./service-watcher.sh $pid&

 # -filter:a loudnorm=I=-14:LRA=1:dual_mono=true:tp=-1
WebPage="http://player.iranseda.ir/live-player/?VALID=TRUE&CH=21&t=b&auto=true&SAVE=TRUE"
Volume=1.0
StreamURL=$(curl -sS "$WebPage" | grep -io 'http.*m3u8' | head -1) 
 
killall -9 ffmpeg 2>/dev/null
./ffmpeg	\
 -nostdin	\
 -re 		\
 -i	"$StreamURL" 	\
 -filter:a "volume=$Volume" \
 -c:a libmp3lame  \
 -loglevel quiet  \
 -f mp3  icecast://source:hackme@192.168.1.9/payam 1>/dev/null
 
#pkill -9 -P $pid
 