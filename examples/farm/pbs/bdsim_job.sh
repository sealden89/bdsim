#!/bin/sh

echo "PBS Job ID  = "$PBS_JOBID

# environment - source bdsim, geant4, root etc
source /home/accsoft/accsoft-eb.sh

# echo inputs
date
echo "input dir   = "${INPUTDIR}
echo "input file  = "${INPUTFILE}
echo "output dir  = "${OUTPUTDIR}
echo "output file = "${OUTPUTFILENAME}
echo "seed value  = "${SEEDVALUE}
echo "ngenerate   = "${NGEN}

echo ""
hostname
echo ""

# make a working directory on the node for this job
JOBDIR=/data/$USER/job_$PBS_JOBID
mkdir -p $JOBDIR
cd $JOBDIR

# prepare cleanup trap to return files in case job is killed
trap "cp $JOBDIR/* ${OUTPUTDIR}/. ; rm -rf $JOBDIR; exit;" SIGTERM SIGINT SIGHUP

# run bdsim
bdsim  --file=${INPUTDIR}/${INPUTFILE} --outfile=$JOBDIR/${SEEDVALUE}_raw --batch --seed=${SEEDVALUE} --ngenerate=${NGEN} 
echo "job finished"
date

# copy generated output to desired output directory
cp $JOBDIR/*.root ${OUTPUTDIR}/.

if [ $? != 0 ]; then
    echo "copy failed"
    exit 1
fi

# clean up temporary files
rm -rf $JOBDIR
if [ $? == 0 ]; then
    echo "tidied up files on node"
fi
