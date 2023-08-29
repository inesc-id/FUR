#!/bin/sh

scriptsfolder=$1
resultsfolder=$2/runs
summaryfolder=$2/summary
plotsfolder=$2/plots


#name of the backend folder
#backends[1]="htm-sgl"
#backends[2]="p8tm-si"
#backends[3]="p8tm"
#backends[4]="tinystm"
#backends[1]="p8tm-psi-v2"
#backends[2]="p8tm-si-v2"
#backends[3]="htm-sgl"
backends[1]="spht"
backends[2]="p8tm-psi-v2"
backends[3]="p8tm-si-v2"
backends[4]="p8tm-psi-v2-fi"
backends[5]="htm-sgl"

#name of the benchmark configuration
benchmarks[1]="tpcc-std-w4"
benchmarks[2]="tpcc-std-w32"
benchmarks[3]="tpcc-ro-w4"
benchmarks[4]="tpcc-ro-w32"
benchmarks[5]="tpcc-hr-w4"
benchmarks[6]="tpcc-hr-w32"

for c in 1 2 4 #1 2 3 4
do
	analyzer="results_analyzer.py"
	for cm in 1 #1
	do
		for b in 2 #4 6 #1 2 3 4 5 6
		do
	    		echo "collecting results for ${benchmarks[$b]}-${backends[$c]}-$cm"
	    		python $scriptsfolder/$analyzer ${benchmarks[$b]}-${backends[$c]}-$cm $resultsfolder > $summaryfolder/${benchmarks[$b]}-${backends[$c]}-$cm
	    	done
	done
done

#for b in 1 2 3 4
#do
#	bash $scriptsfolder/plot.sh $plotsfolder ${benchmarks[$b]} $summaryfolder/${benchmarks[$b]}
#done


