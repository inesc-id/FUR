DATA_FOLDER=/home/miguel.figueiredo/projs/PersistentSiHTM/data/Feb24-1500
WORKSPACE=/home/miguel.figueiredo/projs/PersistentSiHTM/POWER8TM/
mkdir -p $DATA_FOLDER/summary/
GNUPLOT_PS_DIR=/home/miguel.figueiredo/code/gnuplot/gnuplot/term/PostScript/ bash analyze_plot.sh . $DATA_FOLDER
