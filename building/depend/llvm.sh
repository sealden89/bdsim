#!/bin/sh

source ./setup.sh
source ./setup_gcc.sh

DLDURL=http://llvm.org/releases/3.7.0/
DLDFILE=llvm-3.7.0.src.tar
PKGNAME=llvm
LOG=$BASEDIR/$PKGNAME.log
echo $LOG

##################################################
# llvm
##################################################
cd $BASEDIR
echo "Downloading $PKGNAME" | tee $LOG
wget --quiet $DLDURL/$DLDFILE.xz
unxz $DLDFILE.xz
mv  $DLDFILE ./src/

echo "Unpacking $PKGNAME" | tee -a $LOG
rm -rf $BASEDIR/build/$PKGNAME
mkdir $BASEDIR/build/$PKGNAME
tar xf $BASEDIR/src/$DLDFILE -C ./build/llvm --strip-components=1

rm -rf $BASEDIR/build/$PKGNAME-build
mkdir $BASEDIR/build/$PKGNAME-build
cd $BASEDIR/build/$PKGNAME-build

echo "CMake configuring $PKGNAME" | tee -a $LOG
cmake ../$PKGNAME -DCMAKE_INSTALL_PREFIX=$INSTALLDIR/ -DCMAKE_CXX_LINK_FLAGS="-L$INSTALLDIR/lib64 -L$INSTALLDIR/lib -Wl,-rpath,$INSTALLDIR/lib64,-rpath,$INSTALLDIR/lib">> $LOG

echo "CMake build and install $PKGNAME" | tee -a $LOG
make -j$NCPU install >> $LOG
