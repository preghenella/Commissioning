#! /usr/bin/env bash

if [ $# -ne 1 ]; then
    echo "usage: ./epn2raw.sh [filename.tf]"
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "cannot find file: $1"
    exit 1
fi

if [ ! -f "$1.info" ]; then
    echo "cannot find file: $1.info"
    exit 1
fi

execname=.$1.epn2raw.exec
rm -rf $execname
touch $execname
chmod +x $execname
echo "#! /usr/bin/env bash" >> $execname
echo "awk 'NR>1 {print \$10, \$11}' $1.info | xargs -n2 sh -c 'dd bs=\$2 skip=\$1 count=1 iflag=skip_bytes if=$1' sh > $1.raw" >> .$1.epn2raw.exec

./$execname &> /dev/null && rm -rf $execname
