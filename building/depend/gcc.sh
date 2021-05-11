#!/bin/sh

source ./setup.sh

DLDURL=http://nl.mirror.babylon.network/gcc/releases/gcc-4.9.3
DLDFILE=gcc-4.9.3.tar.gz
PKGNAME=gcc
LOG=$BASEDIR/$PKGNAME.log
echo $LOG

##################################################
# GCC
##################################################
cd $BASEDIR
echo "Downloading $PKGNAME" | tee $LOG
wget --quiet $DLDURL/$DLDFILE
rm -rf $BASEDIR/src/$DLDFILE
mv $DLDFILE ./src/

echo "Unpacking $PKGNAME" | tee -a $LOG
rm -rf $BASEDIR/build/$PKGNAME
mkdir $BASEDIR/build/$PKGNAME
tar zfx $BASEDIR/src/$DLDFILE -C ./build/$PKGNAME --strip-components=1
cd $BASEDIR/build/$PKGNAME
echo "Downloading prerequisites $PKGNAME" | tee -a $LOG
./contrib/download_prerequisites >> $LOG

rm -rf $BASEDIR/build/$PKGNAME-build
mkdir $BASEDIR/build/$PKGNAME-build
cd $BASEDIR/build/$PKGNAME-build/

echo "Configuring $PKGNAME" | tee -a $LOG
../gcc/configure --prefix=$INSTALLDIR --enable-languages=c,c++,fortran,go  >> $LOG

echo "Building $PKGNAME" | tee -a $LOG
make -j$NCPU >> $LOG

echo "Installing $PKGNAME" | tee -a $LOG
make -j$NCPU install >> $LOG

