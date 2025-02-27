#!/bin/bash

DIR="~"
NAME="PersistentSiHTM"
NODE="Power8"

DM=$DIR/$NAME

if [[ $# -gt 0 ]] ; then
	NODE=$1
fi

ssh $NODE "mkdir $DIR ; mkdir $DM "

rsync -avz . --exclude plots_scripts/data/ --exclude .git $NODE:$DM

