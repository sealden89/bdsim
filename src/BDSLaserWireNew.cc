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
#include "BDSColours.hh"
#include "BDSLaser.hh"
#include "BDSLaserWireNew.hh"
#include "BDSLogicalVolumeLaser.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"
#include "BDSWireScanner.hh"

BDSLaserWireNew::BDSLaserWireNew(G4String         nameIn,
				 G4double         lengthIn,
				 BDSBeamPipeInfo* beamPipeInfoIn,
				 BDSLaser*        laserIn,
				 G4double         wireLengthIn,
				 G4double         wireAngleIn,
				 G4ThreeVector    wireOffsetIn,
				 G4Colour*        wireColourIn):
  BDSWireScanner(nameIn, lengthIn, beamPipeInfoIn, nullptr, 10*laserIn->Sigma0(),
		 wireLengthIn, wireAngleIn, wireOffsetIn, wireColourIn),
  laser(laserIn)
{
  // override wireMaterial now, which was set to nullptr
  wireMaterial = BDSMaterials::Instance()->GetMaterial("LaserVac");
}

BDSLaserWireNew::~BDSLaserWireNew()
{
  delete laser;
}

G4LogicalVolume* BDSLaserWireNew::BuildWireLV(G4VSolid* solid)
{
  BDSLogicalVolumeLaser* wireLV = new BDSLogicalVolumeLaser(solid,
							    wireMaterial,
							    name + "_laserwire_lv",
							    laser);
  RegisterLogicalVolume(wireLV);
  return wireLV;
}
