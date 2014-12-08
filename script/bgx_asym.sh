#!/bin/bash

bgx_update() {
    bgxoldgrp=${bgxgrp}
    bgxgrp=""
    ((bgxcount = 0))
    bgxjobs=" $(jobs -pr | tr '\n' ' ')"

    for bgxpid in ${bgxoldgrp} ; do
        echo "${bgxjobs}" | grep " ${bgxpid} " >/dev/null 2>&1
        if [[ $? -eq 0 ]] ; then
            bgxgrp="${bgxgrp} ${bgxpid}"
            ((bgxcount = bgxcount + 1))
        fi
    done
}


bgx_limit() {
    bgxmax=$1 ; shift
    bgx_update

    while [[ ${bgxcount} -ge ${bgxmax} ]] ; do
        sleep 1
        bgx_update
    done

    if [[ "$1" != "-" ]] ; then
        ionice -c2 -n7 $* &
        bgxgrp="${bgxgrp} $!"
        # to avoid clashing while updating run database
        sleep 2
    fi
}


bgx_wait() {
   while [[ ${bgxcount} -ne 0 ]] ; do
       oldcount=${bgxcount}

       while [[ ${oldcount} -eq ${bgxcount} ]] ; do
           sleep 1
           bgx_update;
       done
   done
}
