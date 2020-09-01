#!/bin/sh

# bdsim individual job script
echo "PBS Job ID  = "$PBS_JOBID

# environment
source /home/accsoft/accsoft-eb-develop-13julg44crmc.sh

# echo inputs
date
echo "input dir   = "${INPUTDIR}
echo "input file  = "${INPUTFILE}
echo "output dir  = "${OUTPUTDIR}
echo "output file = "${OUTPUTFILENAME}
echo "seed value  = "${SEEDVALUE}
echo "ngenerate   = "${NGEN}

# echo hostname
echo ""
hostname
echo ""

# make a working directory on the node for this job
JOBDIR=/data/$USER/job_$PBS_JOBID
mkdir -p $JOBDIR
cd $JOBDIR

# prepare cleanup trap
trap "cp $JOBDIR/* ${OUTPUTDIR}/. ; rm -rf $JOBDIR; exit;" SIGTERM SIGINT SIGHUP

# run crmc
crmc -o hepmc -s ${SEEDVALUE} -n ${NGEN} -m 6 -p 6500 -P -6500 -f $JOBDIR/crmc_file_${SEEDVALUE}.dat --config $EBROOTCRMC/crmc.param

# run bdsim
bdsim  --file=${INPUTDIR}/${INPUTFILE} --outfile=$JOBDIR/${SEEDVALUE}_raw --batch --seed=${SEEDVALUE} --ngenerate=${NGEN} --distrFile=$JOBDIR/crmc_file_${SEEDVALUE}.dat 
echo "job finished"
date

# run rebdsim
rebdsimHistoMerge $JOBDIR/${SEEDVALUE}_raw.root $JOBDIR/${SEEDVALUE}_histos.root
#rebdsim ${ANALYSISFILE} $JOBDIR/bdsim_${OUTPUTFILENAME}.root $JOBDIR/re_${OUTPUTFILENAME}.root

# copy generated output to desired output directory
cp $JOBDIR/*.root ${OUTPUTDIR}/.
cp $JOBDIR/crmc_file* ${OUTPUTDIR}/.

if [ $? != 0 ]; then
    echo "copy failed"
    exit 1
fi

# clean up temporary files
rm -rf $JOBDIR
if [ $? == 0 ]; then
    echo "tidied up files on node"
fi
