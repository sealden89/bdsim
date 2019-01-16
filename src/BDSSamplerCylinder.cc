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
#include "BDSExtent.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDManager.hh"

#include "globals.hh" // geant types / globals
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"

BDSSamplerCylinder::BDSSamplerCylinder(G4String      nameIn,
				       G4double      length,
				       G4double      radius):
  BDSSampler(nameIn)
{
  containerSolid = new G4Tubs(nameIn + "_solid",    // name
			      radius - 1*CLHEP::um, // inner radius
			      radius,               // outer radius
			      length*0.5,           // half length
			      0,                    // start angle
			      CLHEP::twopi);        // sweep angle

  SetExtent(BDSExtent(radius, radius, length*0.5));

  CommonConstruction();

  containerLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->GetSamplerCylinderSD());
}
