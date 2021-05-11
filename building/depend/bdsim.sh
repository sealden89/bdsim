#!/bin/sh

source ./setup.sh
source ./setup_gcc.sh

PKGNAME=bdsim
LOG=$BASEDIR/$PKGNAME.log

cd $BASEDIR

rm -rf $LOG

rm -rf $BASEDIR/build/$PKGNAME-build
mkdir $BASEDIR/build/$PKGNAME-build
cd $BASEDIR/build/$PKGNAME-build

echo `pwd` >> $LOG

echo "Cmake $PKGNAME" | tee -a $LOG
cmake -DCMAKE_CXX_LINK_FLAGS="-L$INSTALLDIR/lib64 -L$INSTALLDIR/lib -Wl,-rpath,$INSTALLDIR/lib64,-rpath,$INSTALLDIR/lib" -DCMAKE_INSTALL_PREFIX=$INSTALLDIR/ ../$PKGNAME/ >> $LOG 2>&1


echo "Building $PKGNAME" | tee -a $LOG
make -j$NCPU >> $LOG

echo "Install $PKGNAME" | tee -a $LOG
make -j$NCPU install >> $LOG 
