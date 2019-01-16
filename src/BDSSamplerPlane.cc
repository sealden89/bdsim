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
#include "BDSGlobalConstants.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDManager.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Box.hh"
#include "G4LogicalVolume.hh"

G4double BDSSamplerPlane::chordLength = -1;

BDSSamplerPlane::BDSSamplerPlane(G4String nameIn,
				 G4double boxHalfWidth):
  BDSSampler(nameIn)
{
  containerSolid = new G4Box(nameIn + "_solid",
			     boxHalfWidth,
			     boxHalfWidth,
			     chordLength*0.5);

  SetExtent(BDSExtent(boxHalfWidth, boxHalfWidth, chordLength*0.5));

  CommonConstruction();

  containerLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->GetSamplerPlaneSD());
}
