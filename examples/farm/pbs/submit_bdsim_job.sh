#!/bin/bash

#this file name
#FILENAME=$(basename "$fullfile")
FILENAME=${0##*/}
FILENAME2="${FILENAME%.*}"

# redirect stdout ( > ) into a named pipe ( >() ) running "tee"
exec > >(tee -ia logs/$FILENAME2.log)
exec 2>&1
echo "Logging this script in logs/$FILENAME2.log"

# date of this script running
date

# directory creation function script
source mkoutputdir.sh

BASEDIR=/scratch5/lnevay
INPUTDIR=$BASEDIR/bdsim-lhc-model
INPUTFILE=models/physicsdebris/b1-ip1.gmad
NEWDIR=$BASEDIR/physicsdebris/$(date +"%Y%m%d")
ANALFILE=$INPUTDIR/analysis/aperturehits.txt
NGER=100

# make the output directory
mk_output_dir $NEWDIR
# this create a variable called OUTPUTDIR

# make copy of the model - assuming all inputs in same directory
mkdir -p $OUTPUTDIR/modelcopy
cp $INPUTDIR/models/*.gmad $OUTPUTDIR/modelcopy/

# make a copy of this job submission script
cp ${0##*/} $OUTPUTDIR/modelcopy/

# submit the jobs
for N in {1..10}
do
  qsub -q long -l pmem=1300mb,pvmem=2gb -o ${OUTPUTDIR}/${N}_log.txt -j oe -v INPUTDIR=$INPUTDIR,INPUTFILE=$INPUTFILE,OUTPUTDIR=$OUTPUTDIR,OUTPUTFILENAME=$N,SEEDVALUE=$N,ANALYSISFILE=$ANALFILE,NGEN=$NGER bdsim_job.sh
  sleep 0.3
done

echo "All jobs submitted"
