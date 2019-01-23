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
#include "BDSDebug.hh"
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
#include <vector>

class G4VSensitiveDetector;

BDSGeometryComponent::BDSGeometryComponent(G4VSolid*         containerSolidIn,
					   G4LogicalVolume*  containerLVIn,
					   BDSExtent         extentIn,
					   BDSExtent         innerExtentIn,
					   G4ThreeVector     placementOffsetIn,
					   G4RotationMatrix* placementRotationIn):
  containerSolid(containerSolidIn),
  containerLogicalVolume(containerLVIn),
  outerExtent(extentIn),
  innerExtent(innerExtentIn),
  overrideSensitivity(false),
  placementOffset(placementOffsetIn),
  placementRotation(placementRotationIn),
  allBiasingVolumes(nullptr)
{;}

BDSGeometryComponent::BDSGeometryComponent(const BDSGeometryComponent& component):
  containerSolid(component.containerSolid),
  containerLogicalVolume(component.containerLogicalVolume),
  outerExtent(component.outerExtent),
  innerExtent(component.innerExtent),
  overrideSensitivity(component.overrideSensitivity),
  placementOffset(component.placementOffset),
  placementRotation(component.placementRotation)
{
  if (component.allBiasingVolumes)
    {// create new vector<>*
      allBiasingVolumes = new std::vector<G4LogicalVolume*>(*component.allBiasingVolumes);
    }
  else
    {allBiasingVolumes = component.allBiasingVolumes;} // nullptr

}

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
  delete allBiasingVolumes;
}

void BDSGeometryComponent::InheritExtents(BDSGeometryComponent const * const anotherComponent)
{
  outerExtent = anotherComponent->GetExtent();
  innerExtent = anotherComponent->GetInnerExtent();
}

void BDSGeometryComponent::RegisterDaughter(BDSGeometryComponent* anotherComponent)
{
  if (std::find(allDaughters.begin(), allDaughters.end(), anotherComponent) == allDaughters.end())
    {allDaughters.push_back(anotherComponent);}
}

void BDSGeometryComponent::RegisterSolid(G4VSolid* solid, G4bool internalCheck)
{
  if (std::find(allSolids.begin(), allSolids.end(), solid) == allSolids.end())
    {allSolids.push_back(solid);} // not found so register it
  else if (internalCheck)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "just an internal to check that this solid was registered" << G4endl;
#endif
      return;
    }
#ifdef BDSDEBUG
  else
    {
      // found - so don't register it
      G4cout << __METHOD_NAME__ << "warning - solid \""
	     << solid->GetName()
	     << "\" already in this geometry component \"";
      if (containerSolid)
	{G4cout << containerSolid->GetName();}
      else
	{G4cout << " INVALID CONTAINER ";}
      G4cout << "\"" << G4endl;
    }
#endif
}

void BDSGeometryComponent::RegisterSolid(const std::vector<G4VSolid*>& solids)
{
  for (auto solid : solids)
    {RegisterSolid(solid);}
}

void BDSGeometryComponent::RegisterLogicalVolume(G4LogicalVolume* logicalVolume, G4bool internalCheck)
{
  // only register it if it doesn't exist already
  // note search the vector each time something is added is quite computationally expensive
  // but will protect against resetting sensitivity and possibly seg faults by doulby registered
  // logical volumes.  Also, the number of volumes should be < 20 (at maximum) and is only done
  // once at construction time so not as bad as it could be.
  if (std::find(allLogicalVolumes.begin(), allLogicalVolumes.end(), logicalVolume) == allLogicalVolumes.end())
    {allLogicalVolumes.push_back(logicalVolume);} // not found so register it
  else if (internalCheck)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "just an internal to check that this logical volume was registered" << G4endl;
#endif
      return;
    }
#ifdef BDSDEBUG
  else
    {
      // found - so don't register it
      G4cout << __METHOD_NAME__ << "warning - logical volume \""
	     << logicalVolume->GetName()
	     << "\" already in this geometry component \"";
      if (containerSolid)
	{G4cout << containerSolid->GetName();}
      else
	{G4cout << " INVALID CONTAINER ";}
      G4cout << "\"" << G4endl;
    }
#endif
}

void BDSGeometryComponent::RegisterLogicalVolume(const std::vector<G4LogicalVolume*>& logicalVolumes)
{
  for (auto lv : logicalVolumes)
    {RegisterLogicalVolume(lv);}
}

void BDSGeometryComponent::RegisterPhysicalVolume(G4VPhysicalVolume* physicalVolume, G4bool internalCheck)
{
  // only register it if it doesn't exist already
  if (std::find(allPhysicalVolumes.begin(), allPhysicalVolumes.end(), physicalVolume) == allPhysicalVolumes.end())
    {allPhysicalVolumes.push_back(physicalVolume);} // not found so register it
  else if (internalCheck)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "just an internal to check that this physical volume was registered" << G4endl;
#endif
      return;
    }
#ifdef BDSDEBUG
  else
    {
      // found - so don't register it
      G4cout << __METHOD_NAME__ << "warning - physical volume \""
	     << physicalVolume->GetName()
	     << "\" already in this geometry component \"";
      if (containerSolid)
	{G4cout << containerSolid->GetName();}
      else
	{G4cout << " INVALID CONTAINER ";}
      G4cout << "\"" << G4endl;
    }
#endif
}

void BDSGeometryComponent::RegisterPhysicalVolume(std::vector<G4VPhysicalVolume*> physicalVolumes)
{
  std::vector<G4VPhysicalVolume*>::iterator it = physicalVolumes.begin();
  for (; it != physicalVolumes.end(); ++it)
    {RegisterPhysicalVolume(*it);}
}

void BDSGeometryComponent::RegisterRotationMatrix(G4RotationMatrix* rotationMatrix, G4bool internalCheck)
{
  // only register it if it doesn't exist already
  if (std::find(allRotationMatrices.begin(), allRotationMatrices.end(), rotationMatrix) == allRotationMatrices.end())
    {allRotationMatrices.push_back(rotationMatrix);} // not found so register it
  else if (internalCheck)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "just an internal to check that this rotation Matrix was registered" << G4endl;
#endif
      return;
    }
#ifdef BDSDEBUG
  else
    {
      // found - so don't register it
      G4cout << __METHOD_NAME__ << "warning - rotation matrix:"
	     << *rotationMatrix << "already registered in this geometry component \"";
      if (containerSolid)
	{G4cout << containerSolid->GetName();}
      else
	{G4cout << " INVALID CONTAINER ";}
      G4cout << "\"" << G4endl;
    }
#endif
}

void BDSGeometryComponent::RegisterRotationMatrix(std::vector<G4RotationMatrix*> rotationMatrices)
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
  RegisterLogicalVolume(sensitiveVolume, true);

  // this may overwrite it if it's already in the map
  sensitivity[sensitiveVolume] = sensitivityType;
}
void BDSGeometryComponent::RegisterSensitiveVolume(const std::vector<G4LogicalVolume*>& sensitiveVolumes,
						   BDSSDType sensitivityType)
{
  for (auto sv : sensitiveVolumes)
    {RegisterSensitiveVolume(sv, sensitivityType);}
}

void BDSGeometryComponent::RegisterSensitiveVolume(const std::map<G4LogicalVolume*, BDSSDType>& sensitiveVolumes)
{
  sensitivity.insert(sensitiveVolumes.begin(), sensitiveVolumes.end());
}



void BDSGeometryComponent::RegisterVisAttributes(G4VisAttributes* visAttribute, G4bool internalCheck)
{
  if (std::find(allVisAttributes.begin(), allVisAttributes.end(), visAttribute) == allVisAttributes.end())
    {allVisAttributes.push_back(visAttribute);} // not found so register it
  else if (internalCheck)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "just an internal to check that this visualisation attribute was registered" << G4endl;
#endif
      return;
    }
#ifdef BDSDEBUG
  else
    {
      // found - so don't register it
      G4cout << __METHOD_NAME__ << "warning - visualisation attribute: "
	     << visAttribute
	     << " already in this geometry component \"";
      if (containerSolid)
	{G4cout << containerSolid->GetName();}
      else
	{G4cout << " INVALID CONTAINER ";}
      G4cout << "\"" << G4endl;
    }
#endif
}

void BDSGeometryComponent::RegisterVisAttributes(std::vector<G4VisAttributes*> visAttributes)
{
  for (auto va : visAttributes)
    {RegisterVisAttributes(va);}
}

void BDSGeometryComponent::RegisterUserLimits(G4UserLimits* userLimit, G4bool internalCheck)
{
  if (std::find(allUserLimits.begin(), allUserLimits.end(), userLimit) == allUserLimits.end())
    {allUserLimits.push_back(userLimit);} // not found so register it
  else if (internalCheck)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "just an internal to check that this user limit was registered" << G4endl;
#endif
      return;
    }
#ifdef BDSDEBUG
  else
    {
      // found - so don't register it
      G4cout << __METHOD_NAME__ << "warning - user limits: "
	     << userLimit
	     << " already in this geometry component \"";
      if (containerSolid)
	{G4cout << containerSolid->GetName();}
      else
	{G4cout << " INVALID CONTAINER ";}
      G4cout << "\"" << G4endl;
    }
#endif
}

void BDSGeometryComponent::RegisterUserLimits(std::vector<G4UserLimits*> userLimits)
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

std::vector<G4LogicalVolume*> BDSGeometryComponent::GetAllLogicalVolumes() const
{
  std::vector<G4LogicalVolume*> result(allLogicalVolumes);
  for (auto it : allDaughters)
    {
      auto dLVs = it->GetAllLogicalVolumes();
      result.insert(result.end(), dLVs.begin(), dLVs.end());
    }
  return result;
}

std::vector<G4LogicalVolume*> BDSGeometryComponent::GetAllBiasingVolumes() const
{
  std::vector<G4LogicalVolume*> result;
  if (!allBiasingVolumes)
    {result = allLogicalVolumes;} // this excludes daughters
  else
    {result = std::vector<G4LogicalVolume*>(*allBiasingVolumes);}

  for (auto it : allDaughters) // do the same recusively for daughters
    {
      auto dLVs = it->GetAllBiasingVolumes();
      result.insert(result.end(), dLVs.begin(), dLVs.end());
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
  // if we haven't excluded anything yet, there isn't a separate copy of the logical volumes
  // so create a copy then remove it from the biasing list of lvs.
  if (!allBiasingVolumes)
    {allBiasingVolumes = new std::vector<G4LogicalVolume*>();}

  auto v = allBiasingVolumes;
  v->erase( std::remove(v->begin(), v->end(), lv), v->end() );
}
