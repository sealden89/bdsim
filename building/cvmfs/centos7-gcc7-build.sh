source /cvmfs/sft.cern.ch/lcg/releases/LCG_contrib/gcc/7.3.0/x86_64-centos7/setup.sh
source /cvmfs/sft.cern.ch/lcg/releases/LCG_latestpython3/CMake/3.14.3/x86_64-centos7-gcc7-opt/CMake-env.sh
source /cvmfs/sft.cern.ch/lcg/releases/LCG_latestpython3/flex/2.6.4/x86_64-centos7-gcc7-opt/flex-env.sh
source /cvmfs/sft.cern.ch/lcg/releases/LCG_latestpython3/clhep/2.4.1.3/x86_64-centos7-gcc7-opt/clhep-env.sh
source /cvmfs/sft.cern.ch/lcg/releases/LCG_latestpython3/ROOT/6.20.00/x86_64-centos7-gcc7-opt/ROOT-env.sh
source /cvmfs/sft.cern.ch/lcg/releases/LCG_latestpython3/Geant4/10.06.p01/x86_64-centos7-gcc7-opt/Geant4-env.sh

cmake ../bdsim-git/ -DBISON_EXECUTABLE=/usr/bin/bison -DCMAKE_INSTALL_PREFIX=../bdsim/1.4-develop