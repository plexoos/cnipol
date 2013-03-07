#!/bin/bash

# To run this script in background do
#
# >eval `ssh-agent -s`
# >ssh-add
#
# Then modify the SSH_... variables in this script accordingly
#
# >nohup ./copy_results_pc2pc.sh run12
#

#LOG="/eicdata/eic0005/run12/run_asym_nocron.log"
LOG="/usr/local/polarim/log/run_asym_nocron.log"
CHECKINGPERIOD=30 # in sec

export SSH_AUTH_SOCK=/tmp/ssh-CnylBq9610/agent.9610
export SSH_AGENT_PID=9611

#source /eicdata/eic0005/cnipol/setup12.sh >> ${LOG} 2>&1
source /usr/local/cnipol/setup.sh >> ${LOG} 2>&1

# overwrite the data location
export CNIPOL_DATA_DIR=/usr/local/polarim/data_buffer
echo "overwrite the data location" >> ${LOG}
echo "\$CNIPOL_DATA_DIR=$CNIPOL_DATA_DIR" >> ${LOG}

while true;
do
   #echo "Checking for files..."
   array=(`find ${CNIPOL_DATA_DIR}/ -regex "${CNIPOL_DATA_DIR}/\([0-9]+.[0-9]+\).data$" -printf "%f\n"`)
   #array=(`find ${CNIPOL_DATA_DIR}/ -regex "${CNIPOL_DATA_DIR}/\([0-9]+.[0-9]+\).data$" -printf "%f\n" | awk '{ sub(/.data/, "", $0); printf("%s\n", $0); }'`)

   if [ -z "${array}" ]
   then
      #echo "list empty. sleep for $CHECKINGPERIOD. copy new files from online"
      sleep $CHECKINGPERIOD
      rsync -q --remove-source-files -av bluepc:${CNIPOL_DATA_DIR}/ ${CNIPOL_DATA_DIR}/
      continue
   fi

   echo "---" >> ${LOG}
   date >> ${LOG}
   echo Found new files: ${array[@]} >> ${LOG}
   #echo "---"
   #date
   #echo Found new files: ${array[@]}

   for run_name in "${array[@]}"
   do
      #echo processed item : ${run_name%.data} and removed
      #echo $CNIPOL_DIR/bin/asym --calib --profile --use-db --raw-ext --asym --pmt --kinema -g -r ${run_name%.data}
      echo $CNIPOL_DIR/bin/asym --calib --profile --use-db --raw-ext --asym --pmt --kinema -g -r ${run_name%.data} >> ${LOG}
      $CNIPOL_DIR/bin/asym --calib --profile --use-db --raw-ext --asym --pmt --kinema -g -r ${run_name%.data} >> ${LOG} 2>&1
      sleep 2
      rm ${CNIPOL_DATA_DIR}/${run_name}
      sleep 2
   done

done
