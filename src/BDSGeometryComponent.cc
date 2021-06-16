/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"
#include "BDSSDManager.hh"
#include "BDSSDType.hh"

#include "globals.hh"              // geant4 globals / types
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"

#include <algorithm>
#include <map>
#include <set>

class G4VSensitiveDetector;

BDSGeometryComponent::BDSGeometryComponent(G4VSolid*            containerSolidIn,
					   G4LogicalVolume*     containerLVIn,
					   const BDSExtent&     extentIn,
					   const BDSExtent&     innerExtentIn,
					   const G4ThreeVector& placementOffsetIn,
					   G4RotationMatrix*    placementRotationIn):
  containerSolid(containerSolidIn),
  containerLogicalVolume(containerLVIn),
  outerExtent(extentIn),
  innerExtent(innerExtentIn),
  overrideSensitivity(false),
  placementOffset(placementOffsetIn),
  placementRotation(placementRotationIn),
  lvsExcludedFromBiasing(nullptr)
{;}

BDSGeometryComponent::BDSGeometryComponent(const BDSGeometryComponent& component):
  containerSolid(component.containerSolid),
  containerLogicalVolume(component.containerLogicalVolume),
  outerExtent(component.outerExtent),
  innerExtent(component.innerExtent),
  overrideSensitivity(component.overrideSensitivity),
  placementOffset(component.placementOffset),
  placementRotation(component.placementRotation),
  lvsExcludedFromBiasing(nullptr)
{;}

BDSGeometryComponent::~BDSGeometryComponent()
{
  for (auto daughter : allDaughters)
    {delete daughter;}

  // we let Geant4 handle the deletion of physical volumes, logical volumes and solids
  
  for (auto rm : allRotationMatrices)
    {delete rm;}

  for (auto vis : allVisAttributes)
    {delete vis;}

  for (auto ul : allUserLimits)
    {delete ul;}
  
  delete placementRotation;
}

void BDSGeometryComponent::InheritExtents(BDSGeometryComponent const * const anotherComponent)
{
  outerExtent = anotherComponent->GetExtent();
  innerExtent = anotherComponent->GetInnerExtent();
}

void BDSGeometryComponent::InheritExtents(BDSGeometryComponent const * const anotherComponent,
					  const G4ThreeVector &offset)
{
  outerExtent = anotherComponent->GetExtent().Translate(offset);
  innerExtent = anotherComponent->GetInnerExtent().Translate(offset);
}

void BDSGeometryComponent::RegisterDaughter(BDSGeometryComponent* anotherComponent)
{
  if (std::find(allDaughters.begin(), allDaughters.end(), anotherComponent) == allDaughters.end())
    {allDaughters.insert(anotherComponent);}
}

void BDSGeometryComponent::RegisterSolid(const std::set<G4VSolid*>& solids)
{
  for (auto solid : solids)
    {RegisterSolid(solid);}
}

void BDSGeometryComponent::RegisterLogicalVolume(const std::set<G4LogicalVolume*>& logicalVolumes)
{
  for (auto lv : logicalVolumes)
    {RegisterLogicalVolume(lv);}
}

void BDSGeometryComponent::RegisterPhysicalVolume(const std::set<G4VPhysicalVolume*>& physicalVolumes)
{
  for (auto pv : physicalVolumes)
    {RegisterPhysicalVolume(pv);}
}

void BDSGeometryComponent::RegisterRotationMatrix(const std::set<G4RotationMatrix*>& rotationMatrices)
{
  for (auto rm : rotationMatrices)
    {RegisterRotationMatrix(rm);}
}

void BDSGeometryComponent::RegisterSensitiveVolume(G4LogicalVolume* sensitiveVolume,
						   BDSSDType sensitivityType)
{
  if (!sensitiveVolume)
    {return;} // can't register nullptr
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << sensitiveVolume->GetName() << " : " << sensitivityType << G4endl;
#endif

  // check and ensure the logical volume is registered in this component
  RegisterLogicalVolume(sensitiveVolume);

  // this may overwrite it if it's already in the map
  sensitivity[sensitiveVolume] = sensitivityType;
}

void BDSGeometryComponent::RegisterSensitiveVolume(const std::set<G4LogicalVolume*>& sensitiveVolumes,
						   BDSSDType sensitivityType)
{
  for (auto sv : sensitiveVolumes)
    {RegisterSensitiveVolume(sv, sensitivityType);}
}

void BDSGeometryComponent::RegisterSensitiveVolume(const std::map<G4LogicalVolume*, BDSSDType>& sensitiveVolumes)
{
  sensitivity.insert(sensitiveVolumes.begin(), sensitiveVolumes.end());
}

void BDSGeometryComponent::RegisterVisAttributes(const std::set<G4VisAttributes*>& visAttributes)
{
  for (auto va : visAttributes)
    {RegisterVisAttributes(va);}
}

void BDSGeometryComponent::RegisterUserLimits(const std::set<G4UserLimits*>& userLimits)
{
  for (auto ul : userLimits)
    {RegisterUserLimits(ul);}
}

void BDSGeometryComponent::InheritObjects(BDSGeometryComponent* component)
{
  RegisterSolid(component->GetAllSolids());
  RegisterLogicalVolume(component->GetAllLogicalVolumes());
  RegisterSensitiveVolume(component->GetAllSensitiveVolumes());
  RegisterPhysicalVolume(component->GetAllPhysicalVolumes());
  RegisterRotationMatrix(component->GetAllRotationMatrices());
  RegisterVisAttributes(component->GetAllVisAttributes());
  RegisterUserLimits(component->GetAllUserLimits());
}

std::set<G4LogicalVolume*> BDSGeometryComponent::GetAllLogicalVolumes() const
{
  std::set<G4LogicalVolume*> result(allLogicalVolumes);
  for (auto it : allDaughters)
    {
      auto dLVs = it->GetAllLogicalVolumes();
      result.insert(dLVs.begin(), dLVs.end());
    }
  return result;
}

std::set<G4LogicalVolume*> BDSGeometryComponent::GetAllBiasingVolumes() const
{
  std::set<G4LogicalVolume*> result(allLogicalVolumes);

  for (auto it : allDaughters) // do the same recursively for daughters
    {
      auto dLVs = it->GetAllBiasingVolumes();
      result.insert(dLVs.begin(), dLVs.end());
    }

  if (lvsExcludedFromBiasing)
    {
      for (const auto vol : *lvsExcludedFromBiasing)
	{result.erase(vol);}
    }
  return result;
}

std::map<G4LogicalVolume*, BDSSDType> BDSGeometryComponent::GetAllSensitiveVolumes() const
{
  // start by copy sensitive volumes belonging to this object
  std::map<G4LogicalVolume*, BDSSDType> result(sensitivity);
  for (auto it : allDaughters)
    {
      auto dSVs = it->GetAllSensitiveVolumes();
      result.insert(dSVs.begin(), dSVs.end()); // copy into result map
    }
  return result;
}

void BDSGeometryComponent::AttachSensitiveDetectors()
{
  BDSSDManager* sdm = BDSSDManager::Instance();
  for (auto mapping : sensitivity)
    {mapping.first->SetSensitiveDetector(sdm->SensitiveDetector(mapping.second, !overrideSensitivity));}
  for (auto daughter : allDaughters)
    {daughter->AttachSensitiveDetectors();}
}

void BDSGeometryComponent::ExcludeLogicalVolumeFromBiasing(G4LogicalVolume* lv)
{
  if (allLogicalVolumes.find(lv) == allLogicalVolumes.end())
    {
      throw BDSException(__METHOD_NAME__, "excluding volume \"" + lv->GetName()
			 + "\" from component \"" + GetName()
			 + "\" but it's not a member of this piece of geometry or its daughters.");
    }

  if (!lvsExcludedFromBiasing)
    {lvsExcludedFromBiasing = new std::set<G4LogicalVolume*>();}
  lvsExcludedFromBiasing->insert(lv);
}
