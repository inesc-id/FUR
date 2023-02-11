backend=$1 # e.g: herwl
htm_retries=$2 # e.g.: 5
rot_retries=$3 # e.g.: 2
retry_policy=$4

cp ../../backends/$backend/tm.h code/
cp ../../backends/$backend/thread.c code/
cp ../../backends/$backend/thread.h code/
cp ../../backends/$backend/Makefile .
cp ../../backends/$backend/Makefile.common .
cp ../../backends/$backend/Makefile.flags .
cp ../../backends/$backend/Defines.common.mk .


if [[ $backend == spht ]] ; then
    rm lib/rdtsc.h
    source ../../backends/$backend/copy_spht.sh
fi
cd code;
rm tpcc

if [[ $backend == htm-sgl || $backend == hybrid-norec ]]; then
	make_command="make HTM_RETRIES=-DHTM_RETRIES=$htm_retries RETRY_POLICY=-DRETRY_POLICY=$retry_policy"
else
	make_command="make HTM_RETRIES=-DHTM_RETRIES=$htm_retries ROT_RETRIES=-DROT_RETRIES=$rot_retries  RETRY_POLICY=-DRETRY_POLICY=$retry_policy"
fi
$make_command
