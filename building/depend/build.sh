#!/bin/sh

source ./setup.sh
# check if these directories exist
mkdir $INSTALLDIR
mkdir $SRCDIR
mkdir $BUILDDIR

#./gcc.sh
#./cmake.sh
#./llvm.sh
#./clang.sh
#./python.sh
./libpng.sh
./freetype.sh
./fontconfig.sh
./xml2.sh
./xerces3.sh
./root.sh
./clhep.sh
./geant4.sh
./zlib.sh ! Not sure about this
./bdsim.sh
