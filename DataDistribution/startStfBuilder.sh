#! /usr/bin/env bash

StfBuilder \
    --id stf_builder-0 \
    --transport shmem \
    --detector TOF \
    --dpl-channel-name dpl-chan \
    --channel-config "name=dpl-chan,type=push,method=bind,address=ipc:///tmp/stf-builder-dpl-pipe-0,transport=shmem,rateLogging=1" \
    --channel-config "name=readout,type=pull,method=connect,address=ipc:///tmp/readout-pipe-0,transport=shmem,rateLogging=1"
