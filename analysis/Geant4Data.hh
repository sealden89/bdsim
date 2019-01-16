/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ANALYSISGEANT4DATA_H
#define ANALYSISGEANT4DATA_H

#include "TROOT.h"

#include "BDSOutputROOTGeant4Data.hh"

class TTree;

/**
 * @brief Geant4 Data loader.
 *
 * @author Laurie Nevay.
 */

class Geant4Data
{
public:
  Geant4Data();
  Geant4Data(bool debugIn);
  virtual ~Geant4Data();

  /// Set the branch addresses to address the contents of the file.
  void SetBranchAddress(TTree* t);

  /// Member that ROOT can map file data to locally.
  BDSOutputROOTGeant4Data* geant4Data;

private:
  bool debug;
  
  ClassDef(Geant4Data,1);
};

#endif
