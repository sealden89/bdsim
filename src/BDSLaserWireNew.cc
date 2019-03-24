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
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSColours.hh"
#include "BDSLaser.hh"
#include "BDSLaserWireNew.hh"
#include "BDSLogicalVolumeLaser.hh"
#include "BDSMaterials.hh"
#include "G4PVPlacement.hh"
#include "BDSUtilities.hh"
#include "G4Hype.hh"
#include "G4VisAttributes.hh"
#include "BDSUtilities.hh"
#include "BDSDebug.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "G4IntersectionSolid.hh"




#include "BDSWireScanner.hh"

BDSLaserWireNew::BDSLaserWireNew(G4String         nameIn,
				 G4double         lengthIn,
				 BDSBeamPipeInfo* beamPipeInfoIn,
				 BDSLaser*        laserIn,
				 G4Material*      laserMaterialIn,
				 G4double 	  wireDiameterIn,
				 G4double         wireLengthIn,
				 G4double         wireAngleIn,
				 G4double   	  wireLongitudinalAngleIn,
				 G4ThreeVector    wireOffsetIn,
				 G4Colour*        wireColourIn):
  BDSAcceleratorComponent(nameIn, lengthIn,0, "Laserwire", beamPipeInfoIn),
  laser(laserIn),
  wireDiameter(wireDiameterIn),
  wireLength(wireLengthIn),
  wireAngle(wireAngleIn),
  wireLongitudinalAngle(wireLongitudinalAngleIn),
  wireOffset(wireOffsetIn),
  wireColour(wireColourIn)

{
  // override wireMaterial now, which was set to nullptr
  wireMaterial = BDSMaterials::Instance()->GetMaterial("LaserVac");

  if (wireDiameter <= 0)
    {
      G4cerr << __METHOD_NAME__ << "Error: wireDiameter for \"" << name
	     << "\" is not defined or must be greater than 0" <<  G4endl;
      exit(1);
    }
  
  if (wireLength <= 0)
    {
      G4cerr << __METHOD_NAME__ << "Error: wire for \"" << name << "\" must be > 0." << G4endl;
      exit(1);
    }
  
  G4cout << " \n\n****************************************************************\n"
    " Laser geometry is currently under construction! Overlaps can occur!"
    " ****************************************************************" << G4endl;
  
  // check whether the beam pipe will fit transversely (ignores presumably very small
  // wire diameter). work out end points off wire including length and offset in x,y.
  G4TwoVector offsetXY = G4TwoVector(wireOffset.x(), wireOffset.y());
  G4TwoVector tipTop = G4TwoVector(0, 0.5*wireLength);
  tipTop.rotate(wireAngle);
  G4TwoVector tipBot = G4TwoVector(tipTop);
  tipBot.rotate(CLHEP::pi);
  tipTop += offsetXY;
  tipBot += offsetXY;
  //G4double innerRadius = beamPipeInfo->IndicativeRadiusInner();
  /*if (tipTop.mag() > innerRadius || tipBot.mag() > innerRadius)
    {
    G4cerr << __METHOD_NAME__ << "Error: wire for \"" << name
    << "\" is too big to fit in beam pipe give offsets." << G4endl;
    exit(1);
    }
  */
  if (!wireColour)
    {wireColour = BDSColours::Instance()->GetColour("wirescanner");}
}

BDSLaserWireNew::~BDSLaserWireNew()
{
  delete laser;
}

void BDSLaserWireNew::BuildContainerLogicalVolume()
{
  BDSBeamPipeFactory *factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe *pipe = factory->CreateBeamPipe(name + "_beampipe",
					      chordLength,
					      beamPipeInfo);
  RegisterDaughter(pipe);
  
  // make the beam pipe container, this object's container
  containerLogicalVolume = pipe->GetContainerLogicalVolume();
  containerSolid = pipe->GetContainerSolid();
  
  // register vacuum volume (for biasing)
  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());
  
  // update extents
  InheritExtents(pipe);
  
  // update faces
  SetInputFaceNormal(pipe->InputFaceNormal());
  SetOutputFaceNormal(pipe->OutputFaceNormal());
}

void BDSLaserWireNew::Build()
{
  BDSAcceleratorComponent::Build();
  
  G4VSolid* wire = BuildHyperbolicWireSolid();
  G4LogicalVolume* wireLV = BuildWireLV(wire);
  
  G4ThreeVector blank;
  blank.set(0,0,0);
  
  // visualisation attributes
  
  
  G4VisAttributes* wireVisAttr = new G4VisAttributes(*wireColour);
  wireLV->SetVisAttributes(wireVisAttr);
  RegisterVisAttributes(wireVisAttr);
  // placement
  G4PVPlacement* wirePV = new G4PVPlacement(0,           // rotation
					    blank,        // position
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
  G4Hype* laserwire;
  laserwire = new G4Hype(name + "_laserwire_solid", //name
			 0,                  // inner radius
			 wireDiameter*0.5,   // outer radius
			 0,                  // inner stereo
			 laser->HyperbolicAngle(),     // outer stereo
			 wireLength*0.5);
  RegisterSolid(laserwire);
  
  // placement rotation
  G4RotationMatrix* wireRot = new G4RotationMatrix();
  wireRot->rotateX(wireLongitudinalAngle);
  // want to rotate about unit Z but this has now changed
  wireRot->rotateY(wireAngle);
  wireRot->rotateZ(0);
  RegisterRotationMatrix(wireRot);
  wireColour->SetAlpha(0.5);
  
  auto bpf =  BDSBeamPipeFactory::Instance();
  BDSBeamPipe* intersectionBP = bpf->CreateBeamPipeForVacuumIntersection(name + "_vacuum_intersection",
									 chordLength,
									 GetBeamPipeInfo());
  G4VSolid* vacuumSolid = intersectionBP->GetContainerSolid();
  // do intersection with vacuumSolid
  RegisterSolid(vacuumSolid);
  
  G4VSolid* lasersolid = new G4IntersectionSolid(name + "_laserwire_solid",vacuumSolid,laserwire,wireRot,wireOffset);
  
  RegisterSolid(lasersolid);
  
  return lasersolid;
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
