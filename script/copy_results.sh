#!/bin/bash

LOG="/eic/u/dsmirnov/eic0005/run12/copy_results.log"
CHECKINGPERIOD=600 # in sec

export SSH_AUTH_SOCK=/tmp/ssh-lxuXX32554/agent.32554
export SSH_AGENT_PID=32555

while [ 1 ];
do
   echo "---" >> ${LOG}
   date >> ${LOG}

   rsync --stats --bwlimit=10000 -a /eicdata/eic0005/run12/root/* bluepc:/usr/local/polarim/root/ >> ${LOG} 2>&1

   sleep $CHECKINGPERIOD
done
