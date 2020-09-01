#!/bin/sh

# source environment modules command
source /etc/profile.d/modules.sh

# environment - bdsim, root, geant4 etc
source /scratch2/lnevay/accsoft-eb-develop-7jul20.sh

# echo inputs
echo $1
echo $2
echo $3
echo $4
echo $5
echo $6
INPUTFILE=$1
OUTPUTDIR=$2
OUTPUTFILENAME=$3
ANALFILE=$4
SEEDVALUE=$5
NGEN=$6
echo "input file    = "${INPUTFILE}
echo "output dir    = "${OUTPUTDIR}
echo "output file   = "${OUTPUTFILENAME}
echo "analysis file = "${ANALFILE}
echo "seed value    = "${SEEDVALUE}
echo "ngenerate     = "${NGEN}

date
echo ""
hostname
echo ""

# run bdsim
bdsim  --file=${INPUTFILE} --outfile=${SEEDVALUE}_raw --batch --seed=${SEEDVALUE} --ngenerate=${NGEN}
echo "job finished"
date

# run rebdsim analysis on output from bdsim
rebdsim ${ANALFILE} ${SEEDVALUE}_raw.root ${SEEDVALUE}_ana.root

# copy generated output to desired output directory
mkdir -p ${OUTPUTDIR}
cp *.root ${OUTPUTDIR}/.

# clean up
rm *.root

