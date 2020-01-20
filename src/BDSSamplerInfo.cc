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
#include "BDSSamplerInfo.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

BDSSamplerInfo::BDSSamplerInfo(G4String            nameIn,
			       BDSSampler*         samplerIn,
			       G4Transform3D       transformIn,
			       G4double            sPositionIn,
			       BDSBeamlineElement* elementIn,
			       G4String            uniqueNameIn):
  name(nameIn),
  sampler(samplerIn),
  transform(transformIn),
  transformInverse(transformIn.inverse()),
  sPosition(sPositionIn),
  element(elementIn),
  uniqueName(uniqueNameIn)
{;}
