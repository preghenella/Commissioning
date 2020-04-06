#! /usr/bin/env bash

StfSender \
    --id stf_sender-0 \
    --session default \
    --transport shmem \
    --stand-alone \
    --input-channel-name=downstream \
    --channel-config "name=downstream,type=pull,method=connect,address=ipc:///tmp/stf-pipe-0,transport=shmem,rateLogging=1"
