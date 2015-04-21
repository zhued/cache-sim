#!/bin/bash

TRACES="trace/traces-1M/*.gz"

[[ -d "output" ]] || mkdir output

for trace in $TRACES; do
    tracename=$(basename $trace)
    if [[ -z "$CONFIG" ]]; then
	configname=default
    else
	configname=$(basename $CONFIG)
    fi
    echo "Beginning " $tracename " in " $configname
    outfile="output/${tracename/.gz}-${configname/.txt}.out"
    echo "--------------------------------------------------------------------------------" >> $outfile
    printf "		%s.%s		Simulation Results          \n" ${tracename/.gz} ${configname/.txt} >> $outfile
    echo "--------------------------------------------------------------------------------" >> $outfile
    zcat < $trace | ./cache-sim $config >> $outfile
    echo "Finished " $tracename " in " $configname
done
