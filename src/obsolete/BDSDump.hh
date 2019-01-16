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
#ifndef BDSDUMP_H
#define BDSDUMP_H 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

class BDSDump: public BDSAcceleratorComponent
{
public:
  BDSDump(G4String aName,
	  G4double aLength);
  ~BDSDump();

private:
  virtual void BuildContainerLogicalVolume();

  /// number of total Dumps
  static int nDumps;
};

#endif
