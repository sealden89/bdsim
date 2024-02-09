/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSGap.hh"

#include "G4Box.hh"
#include "G4VisAttributes.hh"

#include "globals.hh" // geant4 globals / types

BDSGap::BDSGap (const G4String&   nameIn,
                G4double   lengthIn,
                G4double   angleIn):
  BDSAcceleratorComponent(nameIn, lengthIn, angleIn, "gap")
{;}

BDSGap::~BDSGap()
{;}

// no geometry to generate
void BDSGap::BuildContainerLogicalVolume()
{;}

void BDSGap::Build()
{;}
