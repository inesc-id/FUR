#!/bin/sh

scriptsfolder=$1
resultsfolder=$2/runs
summaryfolder=$2/summary
plotsfolder=$2/plots

source setup.sh

for c in 1 2 3 4 #5 
do
	analyzer="results_analyzer.py"
	for cm in 1
	do
		for b in 20 21 22 10 11 12 #1 4 7 8 10 11
		do
	    		echo "collecting results for ${benchmarks[$b]}-${backends[$c]}-$cm in $summaryfolder/${benchmarks[$b]}-${backends[$c]}-$cm"
	    		python $scriptsfolder/$analyzer ${benchmarks[$b]}-${backends[$c]}-$cm $resultsfolder > $summaryfolder/${benchmarks[$b]}-${backends[$c]}-$cm
				echo "python $scriptsfolder/$analyzer ${benchmarks[$b]}-${backends[$c]}-$cm $resultsfolder > $summaryfolder/${benchmarks[$b]}-${backends[$c]}-$cm"
	    	done
	done
done

#for b in 1 #4 7 8 10 11
#do
#	bash $scriptsfolder/plot.sh $plotsfolder ${benchmarks[$b]} $summaryfolder/${benchmarks[$b]}
#	echo "bash $scriptsfolder/plot.sh $plotsfolder ${benchmarks[$b]} $summaryfolder/${benchmarks[$b]}"
#done


