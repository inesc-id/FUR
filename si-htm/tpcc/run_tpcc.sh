#!/bin/bash

#./run_tpcc.sh PATH_TO_ROOT_DIRECTORY PATH_TO_RESULTS_FOLDERS MESSAGE_DESCRIBING_RUN [no_random]

workspace=$1
if [ $4 == "no_random" ];then
resultsdir=$2
runsdir=$resultsdir/runs
else
DATE=$(date +%b%d-%H%M)
resultsdir=$2/$DATE
mkdir -p $resultsdir/runs
runsdir=$resultsdir/runs
mkdir $resultsdir/plots
mkdir $resultsdir/summary
echo $3 >> $resultsdir/desc.txt
fi


#name of the backend folder
#backends[1]="htm-sgl"
backends[1]="spht"
backends[2]="p8tm-psi-v2"
backends[3]="p8tm-si-v2"
backends[4]="p8tm-psi-v2-fi"
backends[5]="htm-sgl"
#backends[4]="p8tm-psi-v2-flush"
#backends[5]="p8tm"
#backends[6]="p8tm-psi-v2-hl"
#backends[4]="tinystm"

#name of the benchmark configuration
benchmarks[1]="tpcc-std-w4"
benchmarks[2]="tpcc-std-w32"
benchmarks[3]="tpcc-ro-w4"
benchmarks[4]="tpcc-ro-w32"
benchmarks[5]="tpcc-hr-w4"
benchmarks[6]="tpcc-hr-w32"

#name of the executable
bStr[1]="tpcc"
bStr[2]="tpcc"
bStr[3]="tpcc"
bStr[4]="tpcc"
bStr[5]="tpcc"
bStr[6]="tpcc"

#parameters to pass for the respective benchmark configuration
params[1]="-t5 -w4 -m4 -s4 -d4 -o4 -p43 -r45"
params[2]="-t5 -w32 -m32 -s4 -d4 -o4 -p43 -r45"
params[3]="-t5 -w4 -m4 -s4 -d4 -o80 -p4 -r8"
params[4]="-t5 -w32 -m32 -s4 -d4 -o80 -p4 -r8"
params[5]="-t5 -w4 -m4 -s4 -d3 -o90 -p1 -r2"
params[6]="-t5 -w32 -m32 -s4 -d3 -o90 -p1 -r2"

#values to use for retries
cms[1]="0 10 1"
cms[2]="10 0 1"
#cms[1]="0 2 1"
cms[3]="10 5 1"

wait_until_finish() {
    pid3=$1
    echo "process is $pid3"
    LIMIT=90
    for ((j = 0; j < $LIMIT; ++j)); do
        kill -s 0 $pid3
        rc=$u
        if [[ $rc != 0 ]] ; then
            echo "returning"
            return;
        fi
        sleep 1s
    done
    kill -9 $pid3
}


for b in  2 #4 6 #1 2 3 4 5 6
do
	for c in 2 #1 2 4 #1 2 3 4
	do
		for cm in 1
		do
			cd $workspace;
			cd benchmarks/tpcc
			bash build-tpcc.sh ${backends[$c]} ${cms[$cm]}
			#cd ${bStr[$b]};
			cd code
			for t in 4 #1 2 4 8 16 32 #number of threads
			do
        	    		for a in 1 2 3 4 5 #number of attempts for each run
            			do
                			echo "${benchmarks[$b]} | ${backends[$c]}-${cms[$cm]} | threads $t | attempt $a"
					echo "${benchmarks[$b]} | ${backends[$c]}-${cms[$cm]} | threads $t | attempt $a" >> $resultsdir/desc.txt
                			timeout 4m ./${bStr[$b]} ${params[$b]} -n $t > $runsdir/${benchmarks[$b]}-${backends[$c]}-$cm-$t-$a.data 2> $runsdir/${benchmarks[$b]}-${backends[$c]}-$cm-$t-$a.err &

					pid3=$!
					wait_until_finish $pid3
					wait $pid3
        	        		rc=$?
                			if [[ $rc != 0 ]] ; then
                    				echo "Error within: | ${benchmarks[$b]} | ${backends[$c]}-${cms[$cm]} | threads $t | attempt $a" >> $runsdir/error.out
               				fi
				done
			done
			cd ..
		done
	done
done


exit 0
