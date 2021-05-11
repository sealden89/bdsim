#!/bin/sh

source ./setup.sh
source ./setup_gcc.sh

DLDURL=http://zlib.net/
DLDFILE=zlib-1.2.8.tar.gz
PKGNAME=zlib
LOG=$BASEDIR/$PKGNAME.log

##################################################
# zlib
##################################################
cd $BASEDIR

echo "Downloading $PKGNAME" | tee $LOG
wget --quiet $DLDURL/$DLDFILE
mv $DLDFILE ./src/

echo "Unpacking $PKGNAME" | tee -a $LOG
rm -rf $BASEDIR/build/$PKGNAME
mkdir $BASEDIR/build/zlib
tar zxf $BASEDIR/src/zlib-1.2.8.tar.gz -C ./build/zlib --strip-components=1

cd $BASEDIR/build/$PKGNAME

echo "Configuring $PKGNAME" | tee -a $LOG
../zlib/configure --prefix=$INSTALLDIR >> $LOG

echo "Building $PKGNAME" | tee -a $LOG
make -j$NCPU >> $LOG

echo "Installing $PKGNAME" | tee -a $LOG
make install >> $LOG
