#!/bin/sh

source ./setup.sh
source ./setup_gcc.sh

DLDURL=http://prdownloads.sourceforge.net/libpng/
DLDFILE=libpng-1.6.21.tar.gz
PKGNAME=libpng
LOG=$BASEDIR/$PKGNAME.log


##################################################
# libpng
##################################################
cd $BASEDIR
echo "Downloading $PKGNAME" | tee $LOG
wget --quiet $DLDURL/$DLDFILE?download
mv $DLDFILE?download ./src/$DLDFILE

echo "Unpacking $PKGNAME" | tee -a $LOG
rm -rf $BASEDIR/build/$PKGNAME
mkdir $BASEDIR/build/$PKGNAME
tar zxf $BASEDIR/src/$DLDFILE -C ./build/$PKGNAME --strip-components=1


rm -rf $BASEDIR/build/$PKGNAME-build
mkdir $BASEDIR/build/$PKGNAME-build
cd $BASEDIR/build/$PKGNAME-build

echo "Configuring $PKGNAME" | tee -a $LOG
../$PKGNAME/configure --prefix=$INSTALLDIR >> $LOG

echo "Building $PKGNAME" | tee -a $LOG
make -j$NCPU >> $LOG

echo "Installing $PKGNAME" | tee -a $LOG
make install >> $LOG
