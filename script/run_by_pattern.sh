#!/bin/bash

source $CNIPOL_DIR/script/bgx_asym.sh

RUN_PATTERN=$1

ORIG_WD=`pwd`

cd $CNIPOL_DATA_DIR

RUN_LIST=(`ls $1`)

cd $ORIG_WD

echo "Will process ${#RUN_LIST[@]} runs"

for file_name in ${RUN_LIST[@]}
do
   run_name=${file_name%.data} 
   echo -n "$run_name "
done

echo

for file_name in ${RUN_LIST[@]}
do
   run_name=${file_name%.data} 

   bgx_limit 10 $CNIPOL_DIR/bin/asym --calib --profile --use-db --raw-ext --asym --pmt --kinema --sfx=new -g -r $run_name 
done

# Wait until all processes are finished
bgx_wait
