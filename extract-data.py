#!/usr/bin/env python

import sys
import re

if len(sys.argv) < 2:
    print "Usage: {} [ files to process ]"
    exit(1)

infiles = sys.argv[1:]

print "trace,config,l1dsize,l1dways,l1dblocks,l1isize,l1iways,l1iblocks,l2size,l2ways,l2blocks,extime,refs,rrefs,wrefs,irefs,rcycles,wcycles,icycles,idealmisaligned,l1ihits,l1imisses,l1ikickouts,l1idirtykicks,l1iflushkicks,l1dhits,l1dmisses,l1dkickouts,l1ddirtykicks,l1dflushkicks,l2hits,l2misses,l2kickouts,l2dirtykicks,l2flushkicks,flushes"

for infile in infiles:
    with open(infile, "r") as f:
        lines = f.readlines()

        trace, config = re.search("(\w+)\.(\S*)", lines[1]).group(1, 2)

        l1dsize, l1dways, l1dblocks = re.search('Dcache Size = (\d+) : ways = (\d+) : block size = (\d+)',
                                                lines[5]).group(1, 2, 3)
        l1isize, l1iways, l1iblocks = re.search('Icache Size = (\d+) : ways = (\d+) : block size = (\d+)',
                                                lines[6]).group(1, 2, 3)
        l2size, l2ways, l2blocks = re.search('L2-cache Size = (\d+) : ways = (\d+) : block size = (\d+)',
                                             lines[7]).group(1, 2, 3)

        extime, refs = re.search("Execute Time = (\d+); Total References = (\d+)",
                                 lines[10]).group(1, 2)

        flushtime = re.search("Flush Time = (\d+)", lines[11]).group(1)

        rrefs = re.search("Reads\s*=\s*(\d+)", lines[15]).group(1)
        wrefs = re.search("Writes\s*=\s*(\d+)", lines[16]).group(1)
        irefs = re.search("Inst.\s*=\s*(\d+)", lines[17]).group(1)

        rcycles = re.search("Reads\s*=\s*(\d+)", lines[21]).group(1)
        wcycles = re.search("Writes\s*=\s*(\d+)", lines[22]).group(1)
        icycles = re.search("Inst.\s*=\s*(\d+)", lines[23]).group(1)

        idealmisaligned = re.search("Ideal mis-aligned: Exec. Time = (\d+)", lines[29]).group(1)

        l1ihits, l1imisses = re.search("Hit Count = (\d+)\s*Miss Count = (\d+)", lines[32]).group(1, 2)
        l1ikickouts, l1idirtykicks = re.search("Kickouts = (\d+); Dirty Kickouts = (\d+)",
                                               lines[35]).group(1, 2)
        l1iflushkicks = re.search("Flush Kickouts\s*=\s*(\d+)", lines[36]).group(1)

        l1dhits, l1dmisses = re.search("Hit Count = (\d+)\s*Miss Count = (\d+)", lines[39]).group(1, 2)
        l1dkickouts, l1ddirtykicks = re.search("Kickouts = (\d+); Dirty Kickouts = (\d+)",
                                               lines[42]).group(1, 2)
        l1dflushkicks = re.search("Flush Kickouts\s*=\s*(\d+)", lines[43]).group(1)

        l2hits, l2misses = re.search("Hit Count = (\d+)\s*Miss Count = (\d+)", lines[46]).group(1, 2)
        l2kickouts, l2dirtykicks = re.search("Kickouts = (\d+); Dirty Kickouts = (\d+)",
                                             lines[49]).group(1, 2)
        l2flushkicks = re.search("Flush Kickouts\s*=\s*(\d+)", lines[50]).group(1)
        flushes = re.search("Flushes = (\d+)", lines[54]).group(1)

        print "{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}".format(
            trace, config,
            l1dsize, l1dways, l1dblocks, # 3
            l1isize, l1iways, l1iblocks, # 6
            l2size, l2ways, l2blocks,    # 9
            extime, refs,                # 11
            rrefs, wrefs, irefs,         # 14
            rcycles, wcycles, icycles, # 18
            idealmisaligned,                  # 19
            l1ihits, l1imisses,l1ikickouts,l1idirtykicks, l1iflushkicks, # 24
            l1dhits, l1dmisses,l1dkickouts,l1ddirtykicks, l1dflushkicks, # 29
            l2hits, l2misses,l2kickouts,l2dirtykicks, l2flushkicks, # 34
            flushes)                                                # 35
