#! /usr/bin/env bash

INI="o2-raw-file-reader-workflow_sim.ini"

### run workflow chain with only compressor
if false; then

    o2-raw-file-reader-workflow --message-per-tf --conf $INI \
	| o2-tof-compressor --decoder-verbose --encoder-verbose --checker-verbose
  
fi

### run workflow chain with the inspector at the end
if false; then

    o2-raw-file-reader-workflow --message-per-tf --conf $INI \
	| o2-tof-compressor \
	| o2-tof-compressed-inspector 
    
fi


### run workflow chain with the raw parser at the end
if false; then

    o2-raw-file-reader-workflow --message-per-tf --conf $INI \
	| o2-tof-compressor \
	| o2-dpl-raw-parser --input-spec "x:TOF/CRAWDATA" --log-level 2
    
fi

### run workflow chain with the dpl output proxy
if true; then
    o2-raw-file-reader-workflow --message-per-tf --conf $INI \
	| o2-tof-compressor \
	| o2-dpl-output-proxy --dataspec "downstream:TOF/CRAWDATA" --channel-config "name=downstream,type=push,method=bind,address=ipc:///tmp/stf-pipe-0,rateLogging=1,transport=shmem"

fi
