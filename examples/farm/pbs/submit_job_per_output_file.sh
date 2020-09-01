#!/bin/sh

# directory creation function script
source /home/swalker/bdsim-lhc-model/jobs/mkoutputdir.sh

# inputs
INPUTDIR=/nfs/scratch3/swalker/lhcbb/58m/20160705/
INPUTDIR=/home/swalker/lhcbb/58m/20160719_4/
ANALYSISFILE=/home/swalker/bdsim-lhc-model/analysis/atlas_bb_b1_58mpb_ana.txt

# outputs
NEWDIR=$INPUTDIR/$(date +"%Y%m%d")_analysis

# make the output directory
mk_output_dir $NEWDIR
# this create a variable called OUTPUTDIR

# make a copy of inputs and this job submission script
cp $ANALYSISFILE $OUTPUTDIR/
cp ${0##*/} $OUTPUTDIR/
cp rebdsim_job.sh $OUTPUTDIR/

# make a list of the files to be analysed
FILES=$INPUTDIR/*.root

# analyse each file
for F in $FILES
do
    echo "Processing $F"
    #strip the name off the path and then the extension off the name
    #left with just the name
    FNAME="${F##*/}"
    FNAMETWO=${FNAME%%.*}
    qsub -q long -l pmem=2gb,pvmem=2gb -o ${OUTPUTDIR}/${FNAMETWO}_log.txt -j oe -v ANALYSISFILE=$ANALYSISFILE,DATAFILE=$F,OUTPUTDIR=$OUTPUTDIR,OUTPUTFILENAME=${FNAMETWO}_ana.root rebdsim_job.sh
    sleep 0.5
done

echo "All jobs submitted"
