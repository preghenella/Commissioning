#!/bin/bash

# Getting data
pushd DATA
./getdata.sh
popd

# Creating config file
CONFIGFILE="o2-raw-reader-workflow.ini"
echo "[defaults]" > $CONFIGFILE
echo "dataOrigin = TOF" >> $CONFIGFILE
echo "dataDescription = RAWDATA/emulated.raw" >> $CONFIGFILE

FILES=$(ls DATA/*.bin)
NFILES=0
for i in $FILES; do
	echo "" >> $CONFIGFILE
	echo "[input-${NFILES}]" >> $CONFIGFILE
	echo "filePath = $i" >> $CONFIGFILE
	(( NFILES++ ))
done

# Getting json file
echo "Which is the json config file?"
read IN;
ln -s $IN .


