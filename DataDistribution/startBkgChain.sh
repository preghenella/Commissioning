#! /usr/bin/env bash

./clean.sh

nohup ./startStfSender.sh  &> stfSender.log   & echo "stfSender:   starting" && sleep 5
nohup ./startCompressor.sh &> compressor.log  & echo "compressor:  starting" && sleep 5
nohup ./startStfBuilder.sh &> stfBuilder.log  & echo "stfBuilder:  starting" && sleep 5

echo " --- processes are running in background "
echo " --- you can now start readout.exe "
while true; do
    read -p " --- type Q and press enter to quit : " val
    if [ "$val" != "Q" ]; then
	echo "     invalid choice "
	continue
    fi
    killall StfSender o2-dpl-output-proxy o2-tof-compressor o2-dpl-raw-proxy StfBuilder
    ./clean.sh
    exit
done
