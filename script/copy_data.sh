#!/bin/bash
#
# <remote> is the name of the remote machine
#

LOG="/usr/local/polarim/log/copy_data.log"

echo "---" >> ${LOG}
date >> ${LOG}

ionice -c2 -n7 rsync --bwlimit=1000 -av remote:/path/to/data/*.data /path/to/data/ >> ${LOG} 2>&1
