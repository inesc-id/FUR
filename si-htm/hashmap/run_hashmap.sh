#!/bin/bash

#./run_tpcc.sh PATH_TO_ROOT_DIRECTORY PATH_TO_RESULTS_FOLDERS MESSAGE_DESCRIBING_RUN [no_random]

workspace=$1
rm $resultsdir/desc.txt
if [ $4 == "no_random" ]
then
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

# echo " >>>> resultsdir: $resultsdir"
# echo " >>>> workspace: $workspace"
# echo "arg1: $1, arg2: $2, arg3: $3"
# exit 0

source setup.sh

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


# echo "cd $workspace"
# echo "cd benchmarks/datastructures/"
for c in $backend_config #2 3 4 #5
do
	for cm in $retries_config
	do
		cd $workspace/benchmarks/datastructures/
		bash build-datastructures.sh ${backends[$c]} ${cms[$cm]}
		for b in $benchmark_config #1 4 7 8 10 11
		do
			cd ${bStr[$b]};
			for t in $threads_config #number of threads
			do
				for a in $attempts_config #number of attempts for each run
				do
					echo "${benchmarks[$b]} | ${backends[$c]}-${cms[$cm]} | threads $t | attempt $a"
					echo "${benchmarks[$b]} | ${backends[$c]}-${cms[$cm]} | threads $t | attempt $a" >> $resultsdir/desc.txt
					echo "./${bStr[$b]} ${params[$b]} -n $t > $runsdir/${benchmarks[$b]}-${backends[$c]}-$cm-$t-$a.data 2> $runsdir/${benchmarks[$b]}-${backends[$c]}-$cm-$t-$a.err &"
					./${bStr[$b]} ${params[$b]} -n $t > $runsdir/${benchmarks[$b]}-${backends[$c]}-$cm-$t-$a.data 2> $runsdir/${benchmarks[$b]}-${backends[$c]}-$cm-$t-$a.err &
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
