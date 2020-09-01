#!/bin/sh

# bdsim individual job script
echo "PBS Job ID  = "$PBS_JOBID

# environment - source bdsim, geant4, root etc
source /home/accsoft/accsoft-eb.sh

# echo inputs
date
echo "input file  = "${ANALYSISFILE}
echo "data file   = "${DATAFILE}
echo "output dir  = "${OUTPUTDIR}
echo "output file = "${OUTPUTFILENAME}

echo ""
hostname
echo ""

# make a working directory on the node for this job
JOBDIR=/data/$USER/job_$PBS_JOBID
mkdir -p $JOBDIR
cd $JOBDIR

# prepare cleanup trap to return files in case job is killed
trap "cp $JOBDIR/* ${OUTPUTDIR}/. ; rm -rf $JOBDIR; exit;" SIGTERM SIGINT

# copy the data file to the job dir on the node
cp ${DATAFILE} .

# run rebdsim - only one file here so easy to do *.root
rebdsim ${ANALYSISFILE} "*.root" ${OUTPUTFILENAME}
echo "job finished"
date

# copy generated output to desired output directory
cp ${OUTPUTFILENAME} ${OUTPUTDIR}/.
if [ $? != 0 ]; then
    echo "copy failed"
    exit 1
fi

# clean up temporary files
rm -rf $JOBDIR
if [ $? == 0 ]; then
    echo "Tidied up files on node"
fi
