#!/bin/bash

LOG="/usr/local/polarim/log/copy_data.log"

echo "---" >> ${LOG}
date >> ${LOG}

export SSH_AUTH_SOCK=/tmp/ssh-pppCEO1287/agent.1287
export SSH_AGENT_PID=1288

ionice -c2 -n7 rsync --bwlimit=1000 -av /usr/local/polarim/data/*.data eic0005:/eicdata/eic0005/run12/data/ >> ${LOG} 2>&1
