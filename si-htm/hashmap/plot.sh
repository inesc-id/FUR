#!/bin/sh



/home/miguel.figueiredo/code/gnuplot/gnuplot/src/gnuplot << EOF


reset

set term postscript color eps enhanced 14
set size 0.8,0.7
load "styles.inc"
set xlabel "Threads"
set autoscale
set style fill solid
set xrange [1:]
set yrange [0:]
set key outside top horizontal
#unset key

configs="htm-sgl-1 p8tm-si-1"
titles="HTM SI-HTM"

styles="1 2 3 4 5 7 8 9 10"

set output '$1/$2-throughput.eps'

set ylabel "Throughput (tx/s)"

set logscale x 10
set arrow from 10, graph 0 to 10, graph 1 nohead
set arrow from 20, graph 0 to 20, graph 1 nohead
set arrow from 40, graph 0 to 40, graph 1 nohead
set xtics add ("20" 20)
set xtics add ("40" 40)
set xtics add ("80" 80)

plot for [i=1:words(configs)] '$3-'.word(configs, i) using 1:(\$4/\$2) title word(titles,i) with linespoints ls word(styles,i), \
     for [i=1:words(configs)] '$3-'.word(configs, i) using 1:(\$4/\$2):((\$4-\$5)/(\$2+\$3)):((\$4+\$5)/(\$2-\$3)) notitle with errorbars ls word(styles,i)

#set logscale y 10
#set autoscale

#set output '$1/$2-latency-r.eps'

#set ylabel "Latency-r (s)"

#plot for [i=1:words(configs)] '$3-'.word(configs, i).'-10-0' using 1:54 title word(titles,i) with linespoints ls word(styles,i), \
#     for [i=1:words(configs)] '$3-'.word(configs, i).'-10-0' using 1:54:(\$54-\$55):(\$54+\$55) notitle with errorbars ls word(styles,i)

#set output '$1/$2-latency-w.eps'

#set ylabel "Latency-w (s)"

#plot for [i=1:words(configs)] '$3-'.word(configs, i).'-10-0' using 1:56 title word(titles,i) with linespoints ls word(styles,i), \
#     for [i=1:words(configs)] '$3-'.word(configs, i).'-10-0' using 1:56:(\$56-\$57):(\$56+\$57) notitle with errorbars ls word(styles,i)

unset logscale x
unset logscale y
set yrange [0:]
set autoscale

set xtics rotate by -45
set style histogram rowstacked title offset character 2, 0.25, 0
set style data histogram
set boxwidth 0.8
set xlabel offset character 0, -1

set output '$1/$2-total.eps'
set ylabel "Commits/Aborts"
plot newhistogram 'HTM' lt 1 fs pattern 1, '$3-htm-sgl-1' using 4:xtic(1) t "Commits", '' using 14 t "Aborts", \
newhistogram 'SI-HTM' lt 1 fs pattern 1, '$3-p8tm-si-1' using 4:xtic(1) notitle, '' using 14 notitle, \

set output '$1/$2-aborts.eps'
set ylabel "% Aborts"
plot newhistogram 'HTM' lt 1 fs pattern 1, '$3-htm-sgl-1' using 20:xtic(1) t "transactional", '' using 22 t "non-transactional", '' using 26 t "capacity", \
newhistogram 'SI-HTM' lt 1 fs pattern 1, '$3-p8tm-si-1' using 32:xtic(1) notitle, '' using 40 notitle, '' using 42 notitle, \



set yrange [0:100]

set output '$1/$2-commits.eps'
set ylabel "% Commits"
plot newhistogram 'HTM' lt 1 fs pattern 1, '$3-htm-sgl-1' using 6:xtic(1) t "HTM", '' using 8 t "ROT", '' using 10 t "GL/STM", '' using 12 t "Uninstrumented", \
newhistogram 'SI-HTM' lt 1 fs pattern 1, '$3-p8tm-si-1' using 6:xtic(1) notitle, '' using 8 notitle, '' using 10 notitle, '' using 12 notitle, \



EOF

for i in $1/*.eps
do
	../../epstopdf/epstopdf.pl $i
	rm $i
done
