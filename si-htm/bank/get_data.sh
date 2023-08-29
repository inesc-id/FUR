#!/bin/bash

workspace=/home/miguel.figueiredo/code/si-htm-artifacts/POWER8TM/
dir=/home/miguel.figueiredo/code/si-htm-artifacts/POWER8TM/
resultsdir=/home/miguel.figueiredo/code/data/21-10
scriptsfolder=~/code/si-htm-artifacts/si-htm/bank
analyzer="results_analyzer.py"



mkdir -p $resultsdir/runs
mkdir -p $resultsdir/summary
runsdir=$resultsdir/runs
summaryfolder=$resultsdir/summary
resultsfolder=$resultsdir/runs

echo "teste "$runsdir


#name of the backend folder
backends[1]="spht"
backends[2]="p8tm-psi-v2"
backends[3]="p8tm-si-v2"
backends[4]="p8tm-psi-v2-fi"
backends[5]="htm-sgl"
#backends[3]="p8tm"
#backends[4]="tinystm"




#type of benchmark
benchmarks[1]="bank-w1"
benchmarks[2]="bank-w10"
benchmarks[3]="bank-w100"
benchmarks[4]="hashmap-lowcap-lowcon-u10"
benchmarks[5]="hashmap-lowcap-lowcon-u50"
benchmarks[6]="hashmap-lowcap-lowcon-u90"
benchmarks[7]="hashmap-hirocap-hicon-u10"
benchmarks[8]="hashmap-hirocap-hicon-u50"
benchmarks[9]="hashmap-hirocap-hicon-u90"
benchmarks[10]="hashmap-hirocap-lowcon-u10"
benchmarks[11]="hashmap-hirocap-lowcon-u50"
benchmarks[12]="hashmap-hirocap-lowcon-u90"
benchmarks[13]="hashmap-hirwcap-hicon-u10"
benchmarks[14]="hashmap-hirwcap-hicon-u50"
benchmarks[15]="hashmap-hirwcap-hicon-u90"
benchmarks[16]="hashmap-hirwcap-lowcon-u10"
benchmarks[17]="hashmap-hirwcap-lowcon-u50"
benchmarks[18]="hashmap-hirwcap-lowcon-u90"



#app for benchmark
bStr[1]="bank"
bStr[2]="bank"
bStr[3]="bank"
bStr[4]="bank"
bStr[5]="bank"
bStr[6]="bank"
bStr[7]="bank"
bStr[8]="bank"
bStr[9]="bank"
bStr[10]="bank"
bStr[11]="bank"
bStr[12]="bank"
bStr[13]="bank"
bStr[14]="bank"
bStr[15]="bank"
bStr[16]="bank"


#parameters to pass for the respective benchmark configuration
params[1]="-r 10 -b1"
params[2]="-r 10 -b10"
params[3]="-r 10 -b100"
#params[4]="-u10 -i50000 -b1000 -r50000 -d5000000 -q 1 -y 1"
#params[5]="-u50 -i50000 -b1000 -r50000 -d5000000 -q 1 -y 1"
#params[6]="-u90 -i50000 -b1000 -r50000 -d5000000 -q 1 -y 1"
#params[7]="-u10 -i2000 -b10 -r2000 -d5000000 -q 1 -y 1"
#params[8]="-u50 -i2000 -b10 -r2000 -d5000000 -q 1 -y 1"
#params[9]="-u90 -i2000 -b10 -r2000 -d5000000 -q 1 -y 1"
#params[10]="-u10 -i200000 -b1000 -r200000 -d5000000 -q 1 -y 1"
#params[11]="-u50 -i200000 -b1000 -r200000 -d5000000 -q 1 -y 1"
#params[12]="-u90 -i200000 -b1000 -r200000 -d5000000 -q 1 -y 1"
#params[13]="-u10 -i2000 -b1 -r2000 -d5000000 -q 1 -y 1"
#params[14]="-u50 -i2000 -b1 -r2000 -d5000000 -q 1 -y 1"
#params[15]="-u90 -i2000 -b1 -r2000 -d5000000 -q 1 -y 1"
#params[16]="-u10 -i200000 -b1000 -r200000 -d5000000 -q 1 -y 1"
#params[17]="-u50 -i200000 -b1000 -r200000 -d5000000 -q 1 -y 1"
#params[18]="-u90 -i200000 -b1000 -r200000 -d5000000 -q 1 -y 1"

#values to use for retries
cms[1]="0 10 1"
cms[2]="10 0 1"


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



#for c in 1 2 3 4
#do
#	for b in 1 2 3
#	do
#		for cm in 1
#		do
#			cd $workspace;
#			cd benchmarks/bank2/
#			bash build-bank.sh ${backends[$c]} ${cms[$cm]}
#			cd ${bStr[$b]};
#			for t in 1 2 4 8 16 32 #number of threads
#			do
#        	    		for a in 1 2 3 4 5 #number of attempts for each run
#            			do
#                			echo "${benchmarks[$b]} | ${backends[$c]}-${cms[$cm]} | threads $t | attempt $a"
#					echo "${benchmarks[$b]} | ${backends[$c]}-${cms[$cm]} | threads $t | attempt $a" >> $resultsdir/desc.txt
#                			./${bStr[$b]} ${params[$b]} -n $t > $runsdir/${benchmarks[$b]}-${backends[$c]}-$cm-$t-$a.data 2> $runsdir/${benchmarks[$b]}-${backends[$c]}-$cm-$t-$a.err &
#					pid3=$!
#					wait_until_finish $pid3
#					wait $pid3
#        	        		rc=$?
#                			if [[ $rc != 0 ]] ; then
#                    				echo "Error within: | ${benchmarks[$b]} | ${backends[$c]}-${cms[$cm]} | threads $t | attempt $a" >> $runsdir/error.out
#               				fi
#				done
#			done
#
#				echo "collecting results for ${benchmarks[$b]}-${backends[$c]}-$cm in $summaryfolder/${benchmarks[$b]}-${backends[$c]}-$cm"
#	    		python $scriptsfolder/$analyzer ${benchmarks[$b]}-${backends[$c]}-$cm $resultsfolder > $summaryfolder/${benchmarks[$b]}-${backends[$c]}-$cm
#				echo "python $scriptsfolder/$analyzer ${benchmarks[$b]}-${backends[$c]}-$cm $resultsfolder > $summaryfolder/${benchmarks[$b]}-${backends[$c]}-$cm"
#                        #cd ..
#		done
#	done
#done



for c in 5
do
	for b in 1 2 3
	do
		for cm in 2
		do
			cd $workspace;
			cd benchmarks/bank2/
			bash build-bank.sh ${backends[$c]} ${cms[$cm]}
			cd ${bStr[$b]};
			for t in 1 2 4 8 16 32 #number of threads
			do
        	    		for a in 1 2 3 4 5 #number of attempts for each run
            			do
                			echo "${benchmarks[$b]} | ${backends[$c]}-${cms[$cm]} | threads $t | attempt $a"
					echo "${benchmarks[$b]} | ${backends[$c]}-${cms[$cm]} | threads $t | attempt $a" >> $resultsdir/desc.txt
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

				echo "collecting results for ${benchmarks[$b]}-${backends[$c]}-$cm in $summaryfolder/${benchmarks[$b]}-${backends[$c]}-$cm"
	    		python $scriptsfolder/$analyzer ${benchmarks[$b]}-${backends[$c]}-$cm $resultsfolder > $summaryfolder/${benchmarks[$b]}-${backends[$c]}-$cm
				echo "python $scriptsfolder/$analyzer ${benchmarks[$b]}-${backends[$c]}-$cm $resultsfolder > $summaryfolder/${benchmarks[$b]}-${backends[$c]}-$cm"
                        #cd ..
		done
	done
done


exit 0