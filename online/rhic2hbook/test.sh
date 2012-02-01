
source /usr/local/cnipol/script/setup_online.sh
source $CNIPOL_DIR/online/setup_online.sh

#RUN=14890.311
#RUN=15080.309
#RUN=15080.007
#RUN=15122.101
#RUN=15150.006
#RUN=15172.007
RUN=$1

#POLARIM=polarimeter.blu1
#POLARIM=polarimeter.yel1
POLARIM=$2

POLDIR=/usr/local/polarim
#POLDIR=/home/dsmirnov/cnipol/cnipol/trunk/online
LOGDIR=${POLDIR}/log
PSFILE=${LOGDIR}/${RUN}.ps
#PSFILE=./${RUN}.ps
ALOG=${LOGDIR}/an${RUN}.log
HBOOKDIR=${POLDIR}/hbook
HBOOKFILE=${HBOOKDIR}/${RUN}.hbook
BINDIR=${POLDIR}/bin
MACDIR=${BINDIR}/macro
DATADIR=${POLDIR}/data
DATA=${DATADIR}/${RUN}.data
EMITCMD=/usr/local/polarim/bin/emitscan
ANACMD=$BINDIR/rhic2hbook

echo $RUN $POLARIM

mysendpict() {
    if [ -f $2 ] ; then
        convert $2 -trim ${2/.ps/.gif}
        $BINDIR/sndpic $POLARIM $1 ${2/.ps/.gif}
    else
        $BINDIR/sndpic $POLARIM $1 $BINDIR/failed.gif
    fi
}


$ANACMD -l -s $POLARIM $DATA $HBOOKFILE >> $ALOG 2>&1
echo "Starting lr_spinpat.pl..." >> $ALOG
$MACDIR/lr_spinpat.pl $RUN
echo "Starting pvector.pl..." >> $ALOG
$MACDIR/pvector.pl $RUN
echo "Starting pawX11..." >> $ALOG
export RUN LOGDIR PSFILE HBOOKFILE MACDIR       # no other way to pass arguments to kumac...
pawX11 -n -b $MACDIR/onliplot.kumac >> $ALOG 2>&1
echo "Starting sendpict..." >> $ALOG
mysendpict plotData $PSFILE >> $ALOG 2>&1