#!/bin/bash


EXTLIST="*.dat primary-get* o2digitizerworkflow* *.root *log o2sim* localhost* hitmerger-simdata_* workerlog0"
TOREMOVE=""
for i in $EXTLIST; do 
  if [[ -z $(ls $i) ]]; then
   continue
  fi
  TOREMOVE="$TOREMOVE $(ls $i)"
done

echo $TOREMOVE | xargs
