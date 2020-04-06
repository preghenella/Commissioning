#! /usr/bin/env bash

GEOMETRY="800x800"
FONTSIZE="-fs 6"
HOLD="-hold"
#HOLD=""

### compressor
#xterm -geometry $GEOMETRY -fa 'Monospace' $FONTSIZE $HOLD -e ./startCompressor.sh  & sleep 1
### StfBuilder
xterm -geometry $GEOMETRY -fa 'Monospace' $FONTSIZE $HOLD -e ./startStfBuilder.sh & sleep 1
### readout.exe
xterm -geometry $GEOMETRY -fa 'Monospace' $FONTSIZE $HOLD -e ./startReadout.sh & sleep 1
### wait
wait
### cleanup
rm localhost*
