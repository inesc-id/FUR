#!/bin/sh

scriptsfolder=$1
resultsfolder=$2/runs
summaryfolder=$2/summary
plotsfolder=$2/plots


#name of the backend folder
#backends[1]="spht"
#backends[1]="p8tm-psi-v2-co"
backends[1]="spht"
backends[2]="p8tm-psi-v2"
backends[3]="p8tm-si-v2"
backends[4]="p8tm-psi-v2-fi"
backends[5]="htm-sgl"
backends[6]="htm-sgl-sr"
backends[7]="p8tm-psi-v2-qfi"
#backends[1]="p8tm-psi-v2"
#backends[2]="p8tm-si-v2"
#backends[3]="htm-sgl"
#backends[4]="p8tm-psi-v2-ci-10x"
#backends[5]="p8tm-psi-v2-ci"
#backends[6]="p8tm-psi-v2-ci-100x"
#backends[7]="p8tm-psi-v2-10x"
#backends[8]="p8tm-psi-v2-100x"
#backends[9]="spht"
#backends[3]="p8tm-psi-v2-co"
#backends[4]="p8tm-psi-v2-ci"
#backends[5]="p8tm-si"
#backends[4]="tinystm"

#name of the benchmark configuration
benchmarks[1]="hashmap-lowcap-hicon-u10"
benchmarks[2]="hashmap-lowcap-hicon-u50"
benchmarks[3]="hashmap-lowcap-hicon-u90"
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
benchmarks[19]="hashmap-htm-test-lowcon-u100"
benchmarks[20]="hashmap-higher-initSize-lowcon-u10"
benchmarks[21]="hashmap-higher-initSize-lowcon-u50"
benchmarks[22]="hashmap-higher-initSize-lowcon-u90"

#name of the benchmark configuration
#benchmarks[1]="hashmap-i500000-b10000-u10"
#benchmarks[2]="hashmap-i500000-b10000-u50"
#benchmarks[3]="hashmap-i500000-b10000-u90"
#benchmarks[4]="hashmap-i25-b1-u10"
#benchmarks[5]="hashmap-i25-b1-u50"
#benchmarks[6]="hashmap-i25-b1-u100"
#benchmarks[7]="hashmap-i50-b1-u10"
#benchmarks[8]="hashmap-i50-b1-u50"
#benchmarks[9]="hashmap-i50-b1-u100"
#benchmarks[10]="hashmap-i75-b1-u10"
#benchmarks[11]="hashmap-i75-b1-u50"
#benchmarks[12]="hashmap-i75-b1-u100"
#benchmarks[13]="hashmap-i100-b1-u10"
#benchmarks[14]="hashmap-i100-b1-u50"
#benchmarks[15]="hashmap-i100-b1-u100"


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


