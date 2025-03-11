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
#include "BDSInterferometerLaser.hh"

#include <G4SubtractionSolid.hh>

#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSLaser.hh"
#include "BDSLogicalVolumeLaser.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"
#include "BDSWireScanner.hh"

#include "globals.hh"
#include "G4Hype.hh"
#include "G4Tubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"
#include "G4VisAttributes.hh"
#include "BDSException.hh"

BDSInterferometerLaser::BDSInterferometerLaser(G4String         nameIn,
				 G4double         lengthIn,
				 BDSBeamPipeInfo* beamPipeInfoIn,
				 BDSLaser*        laserIn,
				 G4double         wireDiameterIn,
				 G4double         wireLengthIn,
				 G4double         wireAngleIn,
				 G4ThreeVector    wireOffsetIn,
				 G4Colour*        wireColourIn,
				 G4double		  lengthSafetyIn):
  BDSAcceleratorComponent(nameIn, lengthIn,0, "laserinterferometer", beamPipeInfoIn),
  laser(laserIn),
  wireDiameter(wireDiameterIn),
  wireLength(wireLengthIn),
  wireAngle(wireAngleIn),
  wireOffset(wireOffsetIn),
  wireColour(wireColourIn),
  lengthSafety(lengthSafetyIn)

{
  wireMaterial = BDSMaterials::Instance()->GetMaterial("LaserVac");
  overlapWireMaterial = BDSMaterials::Instance()->GetMaterial("laserInterferencevac");

  if (wireDiameter <= 0)
    {throw BDSException(__METHOD_NAME__,"W0 or Sigma0 in element " + name +" must be greater than 0.");}

  if (wireLength <= 0)
    {throw BDSException(__METHOD_NAME__,"wireLength in element " + name +" must be greater than 0.");}

  if(wireAngle < CLHEP::pi && wireAngle < -1.0*CLHEP::pi)
    {throw BDSException(__METHOD_NAME__,"laserOffsetTheta in element " + name +" must be between 0 and two pi.");}

  // check whether the beam pipe will fit transversely (ignores presumably very small
  // wire diameter). work out end points off wire including length and offset in x,y.
  G4TwoVector offsetXY = G4TwoVector(wireOffset.x(), wireOffset.y());
  G4TwoVector tipTop = G4TwoVector(0, 0.5*wireLength);
  tipTop.rotate(wireAngle);
  G4TwoVector tipBot = G4TwoVector(tipTop);
  tipBot.rotate(CLHEP::pi);
  tipTop += offsetXY;
  tipBot += offsetXY;

  if (!wireColour)
    {wireColour = BDSColours::Instance()->GetColour("wirescanner");}
}

BDSInterferometerLaser::~BDSInterferometerLaser()
{
  delete laser;
}

void BDSInterferometerLaser::BuildContainerLogicalVolume()
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

void BDSInterferometerLaser::Build()
{
  BDSAcceleratorComponent::Build();

  G4VSolid* overlap = BuildHyperbolicWireSolid();
  G4LogicalVolume* overlapLV = BuildWireLV(overlap,overlapWireMaterial);
  G4VSolid* wire1 = BuildHyperbolicLaser1();
  G4VSolid* wire2 = BuildHyperbolicLaser2();
  G4LogicalVolume* wireLV1 = BuildWireLV(wire1,wireMaterial);
  G4LogicalVolume* wireLV2 = BuildWireLV(wire2,wireMaterial);

  // visualisation attributes
  G4VisAttributes* wireVisAttr = new G4VisAttributes(*wireColour);
  overlapLV->SetVisAttributes(wireVisAttr);
  wireLV1->SetVisAttributes(wireVisAttr);
  wireLV2->SetVisAttributes(wireVisAttr);

	RegisterVisAttributes(wireVisAttr);
  // placement

  G4RotationMatrix* placementWireRot = new G4RotationMatrix();

  G4RotationMatrix* placementWireRot1 = new G4RotationMatrix();

  //placementWireRot->rotateY(CLHEP::pi-wireAngle);
  placementWireRot->rotateZ(-wireAngle/2);
  placementWireRot1->rotateZ(wireAngle/2);
  placementWireRot1->rotateX(CLHEP::pi/2);
  placementWireRot->rotateX(CLHEP::pi/2);

  //placementWireRot->invert();
  G4LogicalVolume* vac = *(GetAcceleratorVacuumLogicalVolumes().begin()); // take the first one
  G4PVPlacement* overlapPV = new G4PVPlacement(placementWireRot1,           // rotation
					    wireOffset,        // position
					    overlapLV,            // its logical volume
					    name + "_overlap_pv", // its name
					    vac,
					    false,                  // no boolean operation
					    0,                      // copy number
					    checkOverlaps);
	G4PVPlacement* wirePV1 = new G4PVPlacement(placementWireRot1,           // rotation
						wireOffset,        // position
						wireLV1,            // its logical volume
						name + "_wire1_pv", // its name
						vac,
						false,                  // no boolean operation
						0,                      // copy number
						checkOverlaps);
	G4PVPlacement* wirePV2 = new G4PVPlacement(placementWireRot,           // rotation
					  wireOffset,        // position
					  wireLV2,            // its logical volume
					  name + "_wire2_pv", // its name
					  vac,
					  false,                  // no boolean operation
					  0,                      // copy number
					  checkOverlaps);


  RegisterPhysicalVolume(overlapPV);
  RegisterPhysicalVolume(wirePV1);
  RegisterPhysicalVolume(wirePV2);


}

G4VSolid *BDSInterferometerLaser::BuildHyperbolicWire(G4double safety,G4String solidName) {
	G4VSolid* laserwire = new G4Hype(name + solidName, //name
						   0,                  // inner radius
						   wireDiameter * 0.5+safety,   // outer radius
						   0,                  // inner stereo
						   laser->HyperbolicAngle(),     // outer stereo
						   wireLength * 0.5);
	return laserwire;
}


G4VSolid* BDSInterferometerLaser::BuildHyperbolicWireSolid()
{
  G4VSolid* laserwire1 = BuildHyperbolicWire(0,"_laserwire1_solid");
  G4VSolid* laserwire2 = BuildHyperbolicWire(0,"_laserwire1_solid");

  RegisterSolid(laserwire1);
  RegisterSolid(laserwire2);


  G4ThreeVector unitLaserZ;
  unitLaserZ.set(0,0,1);

  // placement rotation
  G4RotationMatrix* wireOverlapRot = new G4RotationMatrix();
  wireOverlapRot->rotateY(-wireAngle);
  wireOverlapRot->rotateZ(0);
  wireOverlapRot->invert();
  RegisterRotationMatrix(wireOverlapRot);
  wireColour->SetAlpha(0.5);


  G4VSolid* laserOverlapSolid = new G4IntersectionSolid("_laseroverlap_solid",laserwire1,laserwire2,wireOverlapRot,wireOffset);
  RegisterSolid(laserOverlapSolid);
  G4RotationMatrix* rotBeamPipe = new G4RotationMatrix();
  rotBeamPipe->rotateY(-wireAngle/2);
  rotBeamPipe->rotateZ(CLHEP::pi/2);
  rotBeamPipe->invert();
  RegisterRotationMatrix(rotBeamPipe);

  auto bpf =  BDSBeamPipeFactory::Instance();
  BDSBeamPipe* intersectionBP = bpf->CreateBeamPipeForVacuumIntersection(name + "_vacuum_intersection",
									 chordLength,
									 GetBeamPipeInfo());
  G4VSolid* vacuumSolid = intersectionBP->GetContainerSolid();
  // do intersection with vacuumSolid
  RegisterSolid(vacuumSolid);

  G4VSolid* laserOverlapsolid = new G4IntersectionSolid(name + "_laseroverlap_beampipe_solid",laserOverlapSolid,vacuumSolid,rotBeamPipe,wireOffset);

  RegisterSolid(laserOverlapsolid);

  return laserOverlapsolid;
}

G4VSolid* BDSInterferometerLaser::BuildHyperbolicLaser1()
{
	G4VSolid* laserwire1 = BuildHyperbolicWire(0,"_laser1_solid");
	G4VSolid* laserwire2 = BuildHyperbolicWire(lengthSafety,"_laser2_solid");

	RegisterSolid(laserwire1);
	RegisterSolid(laserwire2);
	G4RotationMatrix* wireRotPositive = new G4RotationMatrix();
	wireRotPositive->rotateY(-wireAngle);
	wireRotPositive->rotateZ(0);
	wireRotPositive->invert();
	RegisterRotationMatrix(wireRotPositive);
	wireColour->SetAlpha(0.5);

	G4VSolid* laser1 = new G4SubtractionSolid(name + "_laser1_difference_solid",laserwire1, laserwire2, wireRotPositive,wireOffset);
    RegisterSolid(laser1);
	G4RotationMatrix* rotBeamPipe = new G4RotationMatrix();
	rotBeamPipe->rotateY(-wireAngle/2);
	rotBeamPipe->rotateX(CLHEP::pi/2);
	rotBeamPipe->invert();
	RegisterRotationMatrix(rotBeamPipe);

	auto bpf =  BDSBeamPipeFactory::Instance();
	BDSBeamPipe* intersectionBP = bpf->CreateBeamPipeForVacuumIntersection(name + "_vacuum1_intersection",
									   chordLength,
									   GetBeamPipeInfo());
	G4VSolid* vacuumSolid = intersectionBP->GetContainerSolid();
	// do intersection with vacuumSolid
	RegisterSolid(vacuumSolid);

	G4VSolid* laser1solid = new G4IntersectionSolid(name + "_laser1_overlap_solid",laser1,vacuumSolid,rotBeamPipe,wireOffset);

	RegisterSolid(laser1solid);

	return laser1solid;


}

G4VSolid* BDSInterferometerLaser::BuildHyperbolicLaser2()
{
	G4VSolid* laserwire1 = BuildHyperbolicWire(0,"_laser2_second_solid");
	G4VSolid* laserwire2 = BuildHyperbolicWire(lengthSafety,"_laser1_second_solid");

	RegisterSolid(laserwire1);
	RegisterSolid(laserwire2);
	G4RotationMatrix* wireRotPositive = new G4RotationMatrix();
	wireRotPositive->rotateY(wireAngle);
	wireRotPositive->rotateZ(0);
	wireRotPositive->invert();
	RegisterRotationMatrix(wireRotPositive);
	wireColour->SetAlpha(0.5);

	G4VSolid* laser2 = new G4SubtractionSolid(name + "_laser2_difference_solid",laserwire1, laserwire2, wireRotPositive,wireOffset);
	RegisterSolid(laser2);

	G4RotationMatrix* rotBeamPipe = new G4RotationMatrix();
	rotBeamPipe->rotateY(wireAngle/2);
	rotBeamPipe->rotateX(CLHEP::pi/2);

	RegisterRotationMatrix(rotBeamPipe);


	auto bpf =  BDSBeamPipeFactory::Instance();
	BDSBeamPipe* intersectionBP = bpf->CreateBeamPipeForVacuumIntersection(name + "_vacuum2_intersection",
									   chordLength,
									   GetBeamPipeInfo());
	G4VSolid* vacuumSolid = intersectionBP->GetContainerSolid();
	// do intersection with vacuumSolid
	RegisterSolid(vacuumSolid);

	G4VSolid* laser2solid = new G4IntersectionSolid(name + "_laser2_overlap_solid",laser2,vacuumSolid,rotBeamPipe,wireOffset);

	RegisterSolid(laser2solid);

	return laser2solid;

}

G4LogicalVolume* BDSInterferometerLaser::BuildWireLV(G4VSolid* solid, G4Material* material)
{
  BDSLogicalVolumeLaser* wireLV = new BDSLogicalVolumeLaser(solid,
							    material,
							    name + "_laserwire_lv",
							    laser);
  RegisterLogicalVolume(wireLV);
  return wireLV;
}


