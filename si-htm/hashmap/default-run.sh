DATA_FOLDER=/home/miguel.figueiredo/projs/PersistentSiHTM/data/Feb17
WORKSPACE=/home/miguel.figueiredo/projs/PersistentSiHTM/POWER8TM/
mkdir -p $DATA_FOLDER/runs/
bash run_hashmap.sh $WORKSPACE $DATA_FOLDER " " no_random
