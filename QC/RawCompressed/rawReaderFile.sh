#!/bin/bash

o2-raw-file-reader-workflow --message-per-tf --conf o2-raw-file-reader-workflow.ini \
	|\
       	o2-tof-compressor
