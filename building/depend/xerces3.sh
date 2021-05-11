#!/bin/sh

source ./setup.sh
source ./setup_gcc.sh

DLDURL=http://mirror.catn.com/pub/apache//xerces/c/3/sources/
DLDFILE=xerces-c-3.1.3.tar.gz
PKGNAME=xerces3
LOG=$BASEDIR/$PKGNAME.log

##################################################
# libxml2
##################################################
cd $BASEDIR

echo "Downloading $PKGNAME" | tee $LOG
wget --quiet $DLDURL/$DLDFILE
mv $DLDFILE ./src/

rm -rf $BASEDIR/build/$PKGNAME
mkdir $BASEDIR/build/$PKGNAME
tar zxf $BASEDIR/src/$DLDFILE -C ./build/$PKGNAME --strip-components=1

echo "Unpacking $PKGNAME" | tee a $LOG
rm -rf $BASEDIR/build/$PKGNAME-build
mkdir $BASEDIR/build/$PKGNAME-build
cd $BASEDIR/build/$PKGNAME-build

echo "Configuring $PKGNAME" | tee -a $LOG
../$PKGNAME/configure --prefix=$INSTALLDIR >> $LOG

echo "Building $PKGNAME" | tee -a $LOG
make -j$NCPU >> $LOG

echo "Installing $PKGNAME" | tee -a $LOG
make install >> $LOG
