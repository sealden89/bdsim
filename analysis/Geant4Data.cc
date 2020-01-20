/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "Geant4Data.hh"
#include "BDSOutputROOTGeant4Data.hh"

#include "TTree.h"

ClassImp(Geant4Data)

Geant4Data::Geant4Data():
  Geant4Data(false)
{;}

Geant4Data::Geant4Data(bool debugIn):
  debug(debugIn)
{
  geant4Data = new BDSOutputROOTGeant4Data();
}

Geant4Data::~Geant4Data()
{
  delete geant4Data;
}

void Geant4Data::SetBranchAddress(TTree* t)
{
  t->SetBranchAddress("Geant4Data.", &geant4Data);
}
