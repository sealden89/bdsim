#!/bin/sh

source ./setup.sh
source ./setup_gcc.sh


DLDURL=https://www.python.org/ftp/python/2.7.11/
DLDFILE=Python-2.7.11.tgz
PKGNAME=python
LOG=$BASEDIR/$PKGNAME.log
echo $LOG

##################################################
# python
##################################################
cd $BASEDIR
echo "Downloading $PKGNAME" | tee $LOG
wget --quiet $DLDURL/$DLDFILE
mv $DLDFILE ./src/

echo "Unpacking $PKGNAME" | tee -a $LOG
rm -rf $BASEDIR/build/$PKGNAME
mkdir $BASEDIR/build/$PKGNAME
tar zxf $BASEDIR/src/$DLDFILE -C ./build/$PKGNAME --strip-components=1


rm -rf $BASEDIR/build/$PKGNAME-build
mkdir $BASEDIR/build/$PKGNAME-build
cd $BASEDIR/build/$PKGNAME-build

echo "Configuring $PKGNAME" | tee -a $LOG
../$PKGNAME/configure --prefix=$INSTALLDIR --enable-unicode=ucs4 --enable-shared LDFLAGS="-L$INSTALLDIR/lib64/ -L$INSTALLDIR/lib/ -Wl,-rpath,$INSTALLDIR/lib64,-rpath,$INSTALLDIR/lib" >> $LOG

echo "Building $PKGNAME" | tee -a $LOG
make -j$NCPU >> $LOG

echo "Installing $PKGNAME" | tee -a $LOG
make -j$NCPU >> $LOG
