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

RUN_PERIOD=$1

LOG="/eic/u/dsmirnov/eic0005/copy_results_pc2pc_${RUN_PERIOD}.log"
CHECKINGPERIOD=3600 # in sec

export SSH_AUTH_SOCK=/tmp/ssh-lKLHL25172/agent.25172
export SSH_AGENT_PID=25173

while [ 1 ];
do
   echo "---" >> ${LOG}
   date >> ${LOG}

   rsync --stats --bwlimit=100000 --exclude=*.root -aO /eicdata/eic0005/${RUN_PERIOD}/root/* pc2pc-phy:/usr/local/polarim/root/ >> ${LOG} 2>&1

   sleep $CHECKINGPERIOD
done
