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
#include "BDSCavity.hh"
#include "BDSCavityFactoryBase.hh"
#include "BDSCavityInfo.hh"
#include "BDSColours.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <vector>

class G4UserLimits;

BDSCavityFactoryBase::BDSCavityFactoryBase()
{
  lengthSafety       = BDSGlobalConstants::Instance()->LengthSafety();
  nSegmentsPerCircle = BDSGlobalConstants::Instance()->NSegmentsPerCircle();
  emptyMaterial      = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
  checkOverlaps      = BDSGlobalConstants::Instance()->CheckOverlaps();
  sensitiveBeamPipe  = BDSGlobalConstants::Instance()->SensitiveBeamPipe();
  sensitiveVacuum    = BDSGlobalConstants::Instance()->SensitiveVacuum();

  CleanUpBase(); // initialise variables
}

BDSCavity* BDSCavityFactoryBase::CreateCavity(G4String             name,
					      G4double             totalChordLength,
					      const BDSCavityInfo* info,
					      G4Material*          vacuumMaterial)
{
  CleanUp();
  G4double containerRadius = CreateSolids(name, totalChordLength, info);
  CreateLogicalVolumes(name, info, vacuumMaterial);
  SetUserLimits(totalChordLength);
  SetVisAttributes();
  PlaceComponents(name);

  BDSExtent ext = BDSExtent(containerRadius, containerRadius, totalChordLength*0.5);
  BDSCavity* result = BuildCavityAndRegisterObjects(ext);

  return result;
}

void BDSCavityFactoryBase::CleanUp()
{
  CleanUpBase();
}

void BDSCavityFactoryBase::CleanUpBase()
{
  // we don't delete any pointers as this factory doesn't own them.
  allSolids.clear();
  allLogicalVolumes.clear();
  allSensitiveVolumes.clear();
  allPhysicalVolumes.clear();
  allRotationMatrices.clear();
  allUserLimits.clear();
  allVisAttributes.clear();

  vacuumSolid    = nullptr;
  cavitySolid    = nullptr;
  containerSolid = nullptr;
  vacuumLV       = nullptr;
  cavityLV       = nullptr;
  containerLV    = nullptr;
}

void BDSCavityFactoryBase::CreateLogicalVolumes(G4String             name,
						const BDSCavityInfo* info,
						G4Material*          vacuumMaterial)
{
  cavityLV = new G4LogicalVolume(cavitySolid,          // solid
				 info->material,       // material
				 name + "_cavity_lv"); // name
  allLogicalVolumes.push_back(cavityLV);
  allSensitiveVolumes.push_back(cavityLV);
  
  vacuumLV = new G4LogicalVolume(vacuumSolid,           // solid
				 vacuumMaterial,        // material
				 name + "_vacuum_lv");  // name
  if (sensitiveVacuum)
    {allSensitiveVolumes.push_back(vacuumLV);}
  allLogicalVolumes.push_back(vacuumLV);

  containerLV = new G4LogicalVolume(containerSolid,
				    emptyMaterial,
				    name + "_container_lv");
}

void BDSCavityFactoryBase::SetUserLimits(G4double length)
{
  auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
  //copy the default and update with the length of the object rather than the default 1m
  G4UserLimits* ul = BDS::CreateUserLimits(defaultUL, length);

  if (ul != defaultUL) // if it's not the default register it
    {allUserLimits.push_back(ul);}

  for (auto lv : allLogicalVolumes)
    {lv->SetUserLimits(ul);}
}

void BDSCavityFactoryBase::SetVisAttributes(G4String colourName)
{
  // visualisation attributes
  auto col = BDSColours::Instance()->GetColour(colourName);
  G4VisAttributes* cavityVis = new G4VisAttributes(*col);
  cavityVis->SetVisibility(true);
  cavityVis->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  cavityLV->SetVisAttributes(cavityVis);
  allVisAttributes.push_back(cavityVis);
  
  // vacuum
  vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());
  // container
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());
}

void BDSCavityFactoryBase::PlaceComponents(G4String name)
{
  G4PVPlacement* vacuumPV = new G4PVPlacement((G4RotationMatrix*)nullptr, // rotation
					      (G4ThreeVector)0,      // position
					      vacuumLV,              // logical Volume to be placed
					      name + "_vacuum_pv",   // placement name
					      containerLV,           // mother volume
					      false,                 // pMany unused
					      0,                     // copy number
					      checkOverlaps);        // check overlaps
  allPhysicalVolumes.push_back(vacuumPV);
  
  G4PVPlacement* cavityPV = new G4PVPlacement((G4RotationMatrix*)nullptr, // rotation
					      (G4ThreeVector)0,  // position
					      cavityLV,              // logical Volume to be placed
					      name + "_cavity_pv",   // placement name
					      containerLV,           // mother volume
					      false,                 // pMany unused
					      0,                     // copy number
					      checkOverlaps);        // check overlaps
  allPhysicalVolumes.push_back(cavityPV);
}

BDSCavity* BDSCavityFactoryBase::BuildCavityAndRegisterObjects(const BDSExtent& extent)
{
  BDSCavity* cavity = new BDSCavity(containerSolid, containerLV,
				    extent, vacuumLV);

  // register objects
  cavity->RegisterSolid(allSolids);
  cavity->RegisterLogicalVolume(allLogicalVolumes); //using geometry component base class method
  if (sensitiveBeamPipe)
    {cavity->RegisterSensitiveVolume(allSensitiveVolumes);}
  cavity->RegisterPhysicalVolume(allPhysicalVolumes);
  cavity->RegisterRotationMatrix(allRotationMatrices);
  cavity->RegisterUserLimits(allUserLimits);
  cavity->RegisterVisAttributes(allVisAttributes);
  
  return cavity;
}
