#!/bin/sh

source ./setup.sh
source ./setup_gcc.sh

DLDURL=http://geant4.cern.ch/support/source/
DLDFILE=geant4.10.02.p03.tar.gz
PKGNAME=geant4
LOG=$BASEDIR/$PKGNAME.log
echo $LOG

cd $BASEDIR

rm -rf $LOG
echo "Downloading $PKGNAME" | tee $LOG
wget --quiet $DLDURL/$DLDFILE
rm -rf ./src/$DLDFILE
mv $DLDFILE ./src/
rm -rf $BASEDIR/build/$PKGNAME
mkdir $BASEDIR/build/$PKGNAME

echo "Unpacking $PKGNAME" | tee -a $LOG 
tar zxf $BASEDIR/src/$DLDFILE -C ./build/$PKGNAME --strip-components=1
rm -rf $BASEDIR/build/$PKGNAME-build
mkdir $BASEDIR/build/$PKGNAME-build
cd $BASEDIR/build/$PKGNAME-build

echo "Cmake $PKGNAME" | tee -a $LOG
cmake -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_SYSTEM_CLHEP=ON -DGEANT4_USE_GDML=ON -DCMAKE_CXX_LINK_FLAGS="-L$INSTALLDIR/lib64 -L$INSTALLDIR/lib -Wl,-rpath,$INSTALLDIR/lib64,-rpath,$INSTALLDIR/lib" -DCMAKE_INSTALL_PREFIX=$INSTALLDIR/ ../$PKGNAME/ >> $LOG 2>&1

echo "Building $PKGNAME" | tee -a $LOG
make -j$NCPU install >> $LOG 2>&1
