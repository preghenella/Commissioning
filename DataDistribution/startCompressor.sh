#! /usr/bin/env bash

o2-dpl-raw-proxy -b --dataspec "A:TOF/RAWDATA" --channel-config "name=readout-proxy,type=pull,method=connect,address=ipc:///tmp/stf-builder-dpl-pipe-0,transport=shmem,rateLogging=1" | o2-tof-compressor
