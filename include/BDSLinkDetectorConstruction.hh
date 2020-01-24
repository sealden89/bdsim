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
#ifndef BDSLINKDETECTORCONSTRUCTION_H
#define BDSLINKDETECTORCONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;

class BDSLinkDetectorConstruction: public G4VUserDetectorConstruction
{
public:
  /// Default constructor
  BDSLinkDetectorConstruction(/* something. special GMAD syntax?*/);

  virtual ~BDSDetectorConstruction();
  virtual G4VPhysicalVolume* Construct();
};

#endif
