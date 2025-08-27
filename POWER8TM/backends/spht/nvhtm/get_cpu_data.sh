#!/bin/bash

cat /proc/cpuinfo | grep ".*MHz" | sed -n '1p' | sed -e 's/clock.*: //g' | sed -e 's/MHz.*//g' > CPU_FREQ_MHZ.txt
echo "$(cat CPU_FREQ_MHZ.txt) * 1000" | bc > CPU_FREQ_kHZ.txt
