Beam Delivery Simulation (BDSIM) Copyright (c) Royal Holloway, University of London 2001 - 2019

General Information
===================

BDSIM version 1.3.1, program for radiation transport in accelerators

The official web site is:
	http://www.pp.rhul.ac.uk/bdsim

Currently maintained and developed by:

Laurie Nevay          <laurie.nevay@rhul.ac.uk> (main contact)
Andrey Abramov        <andrey.abramov.2012@live.rhul.ac.uk>
Stewart Boogert       <stewart.boogert@rhul.ac.uk>
Hector Garcia Morales <hector.garciamorales@rhul.ac.uk>
Stephen Gibson        <stephen.gibson@rhul.ac.uk>
Will Shields          <william.shields.2010@live.rhul.ac.uk>
Jochem Snuverink      <jochem.snuverink@psi.ch>
Stuart Walker         <stuart.walker.2011@live.rhul.ac.uk>

Originally started by G.A. Blair. See manual for full authorship.

History
=======

v1.3.1: 05.03.19
v1.3:   27.02.19
v1.2:   26.08.18
v1.1:   23.05.18
v1.0:   10.04.18
v0.993: 11.12.17
v0.992: 26.07.17
v0.99:  09.06.17
v0.95:  07.11.16
v0.94:  14.09.16
v0.93:  24.08.16
v0.92:  29.03.16
v0.91:  17.12.15
v0.9 :  10.11.15
v0.8 :  10.08.15
v0.7 :  30.06.15
v0.65:  10.04.15
v0.64:  16.02.15
v0.63:  06.02.15
v0.62:  07.08.14
v0.61:  05.08.14
v0.6 :  02.12.13
v0.5 :  30.05.08
v0.4 :  25.02.08
v0.3 :  26.01.07
v0.2 :  18.05.06
v0.1 :  15.11.05 
Beta :  05.05.05

Installation
============

System requirements:

tested on:
    os: Linux (Scientific Linux Cern 6), Mac OS 10.10 onwards
    architecture: x86_64
    compiler: gcc 4.9, clang 6 (fully c++ 11 compliant)
   
mandatory:
   - BISON
   - CLHEP (recommended version 2.1.3.1 or greater)
   - FLEX
   - Geant4 versions Geant4.10 or greater (10.4.p02 recommended)
     Latest patch version recommended of any 10.X version. 
   - ROOT 6

optional: 
   - OpenGL / QT (for interactive visualiser)
   - XercesC3 (for GDML)

To compile, see http://www.pp.rhul.ac.uk/bdsim/manual/installation.html

   %  mkdir build
   %  cd build
   %  cmake ../bdsim
   %  make
   %  make install

Getting Started
===============

To run:
    bdsim [options]

for example: 
    bdsim --file=line.gmad

to see the full list of options:

    bdsim --help


Some input examples are available in ./examples/ directory. 
For more details consult documentation on http://www.pp.rhul.ac.uk/bdsim/manual/

Directories
===========

analysis/        : analysis tools
cmake/           : cmake configuration files
comparator/      : tool for comparison of results
configuration/   : software version information for compilation
convert/         : conversion of coordinates
depend/          : build environment
examples/        : sample lattices
include/         : header files
interpolator/    : tool for field map validation
manual/          : documentation
modules/         : extra modules for user code
parser/          : gmad parser
src/             : source files
src-external/    : third party source code
test/            : various test libraries
utils/           : various "useful" scripts
vis/             : default visualisation files
