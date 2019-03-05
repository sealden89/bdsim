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
#include "G4PVPlacement.hh"
#include "BDSUtilities.hh"
#include "G4Hype.hh"
#include "G4VisAttributes.hh"

#include "BDSWireScanner.hh"

BDSLaserWireNew::BDSLaserWireNew(G4String         nameIn,
				 G4double         lengthIn,
				 BDSBeamPipeInfo* beamPipeInfoIn,
				 BDSLaser*        laserIn,
				 G4double         wireLengthIn,
				 G4double         wireAngleIn,
				 G4ThreeVector    wireOffsetIn,
				 G4Colour*        wireColourIn,
				 G4double 		  laserHyperbolaAngleIn,
				 G4bool 		  hyperboloidIn):
  BDSWireScanner(nameIn, lengthIn, beamPipeInfoIn, nullptr, 10*laserIn->Sigma0(),
		 wireLengthIn, wireAngleIn, wireOffsetIn, wireColourIn),
  laser(laserIn),
  laserHyperbolaAngle(laserHyperbolaAngleIn),
  hyperboloid(hyperboloidIn)

{
  // override wireMaterial now, which was set to nullptr
  wireMaterial = BDSMaterials::Instance()->GetMaterial("LaserVac");
}

BDSLaserWireNew::~BDSLaserWireNew()
{
  delete laser;
}

void BDSLaserWireNew::Build()
{
	BDSAcceleratorComponent::Build();

	G4VSolid*  wire;
 	if(hyperboloid)
	  { wire = BuildHyperbolicWireSolid(); }
	else
	  { wire   = BuildWireSolid(); }
	G4LogicalVolume* wireLV = BuildWireLV(wire);

	// placement rotation
	G4RotationMatrix* wireRot = new G4RotationMatrix();
	wireRot->rotateX(CLHEP::halfpi);
	// want to rotate about unit Z but this has now changed
	wireRot->rotateY(wireAngle);
	RegisterRotationMatrix(wireRot);
    wireColour->SetAlpha(0.2);
	// visualisation attributes
	G4VisAttributes* wireVisAttr = new G4VisAttributes(*wireColour);
	wireLV->SetVisAttributes(wireVisAttr);
	RegisterVisAttributes(wireVisAttr);

	// placement
	G4PVPlacement* wirePV = new G4PVPlacement(wireRot,           // rotation
											  wireOffset,        // position
											  wireLV,            // its logical volume
											  name + "_wire_pv", // its name
											  GetAcceleratorVacuumLogicalVolume(),
											  false,                  // no boolean operation
											  0,                      // copy number
											  checkOverlaps);
	RegisterPhysicalVolume(wirePV);

}

G4VSolid* BDSLaserWireNew::BuildHyperbolicWireSolid()
{
	G4Hype* laserwire = new G4Hype(name +"_laserwire_solid", //name
								   0,                  // inner radius
								   wireDiameter*0.5,   // outer radius
								   0,                  // inner stereo
								   laserHyperbolaAngle,     // outer stereo
								   wireLength*0.5);
	RegisterSolid(laserwire);
	return laserwire;
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
