#!/bin/sh

source ./setup.sh
source ./setup_gcc.sh

DLDURL=https://cmake.org/files/v3.5/
DLDFILE=cmake-3.5.2.tar.gz
PKGNAME=cmake
LOG=$BASEDIR/$PKGNAME.log
echo $LOG

##################################################
# cmake
##################################################
cd $BASEDIR
echo "Downloading $PKGNAME" | tee $LOG
wget --quiet $DLDURL/$DLDFILE
mv $DLDFILE ./src/

echo "Unpacking $PKGNAME" | tee -a $LOG
rm -rf $BASEDIR/build/$PKGNAME
mkdir $BASEDIR/build/$PKGNAME
tar zxf $BASEDIR/src/cmake-3.5.2.tar.gz -C ./build/cmake --strip-components=1
rm -rf $BASEIDR/build/$PKGNAME
mkdir $BASEDIR/build/cmake-build
cd $BASEDIR/build/cmake-build

echo "CMake configuring $PKGNAME" | tee -a $LOG
cmake ../cmake -DCMAKE_INSTALL_PREFIX=$INSTALLDIR/ -DCMAKE_CXX_LINK_FLAGS="-L$INSTALLDIR/lib64 -L$INSTALLDIR/lib -Wl,-rpath,$INSTALLDIR/lib64,-rpath,$INSTALLDIR/lib" >> $LOG

echo "CMake build and install $PKGNAME" | tee -a $LOG
make -j$NCPU install >> $LOG

