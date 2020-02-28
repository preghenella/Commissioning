#!/bin/bash

o2-raw-file-reader-workflow -b --message-per-tf --conf o2-raw-reader-workflow.ini \
       	| \
       	o2-tof-compressor -b \
	| \
	o2-qc -b --config json://${PWD}/tofcompressed.json

