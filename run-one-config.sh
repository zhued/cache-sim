#!/bin/bash

TRACE=$1
CONFIG=$2

[[ -d "output" ]] || mkdir output

tracename=$(basename $TRACE)
if [[ -z "$CONFIG" ]]; then
configname=default
else
configname=$(basename $CONFIG)
fi
echo "Beginning " $tracename " in " $configname
outfile="output/${tracename/.gz}-${configname/.txt}.out"
echo "----------------------------------------------------------------------" >> $outfile
printf "        %s.%s       Simulation Results          \n" ${tracename/.gz} ${configname/.txt} >> $outfile
echo "----------------------------------------------------------------------" >> $outfile
zcat < $TRACE | ./cache-sim $CONFIG >> $outfile
echo "Finished " $tracename " in " $configname

