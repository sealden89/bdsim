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
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSApertureInfo.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineSet.hh"
#include "BDSDebug.hh"
#include "BDSFieldObjects.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4ProductionCuts.hh"
#include "G4Region.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"

#include <cstdio>
#include <map>
#include <set>
#include <vector>

BDSAcceleratorModel* BDSAcceleratorModel::instance = nullptr;

BDSAcceleratorModel* BDSAcceleratorModel::Instance()
{
  if (!instance)
    {instance = new BDSAcceleratorModel();}
  return instance;
}

BDSAcceleratorModel::BDSAcceleratorModel():
  worldPV(nullptr),
  worldLV(nullptr),
  worldSolid(nullptr),
  tunnelBeamline(nullptr),
  placementBeamline(nullptr)
{
  BDSAcceleratorComponentRegistry::Instance();
  BDSPhysicalVolumeInfoRegistry::Instance();
}

BDSAcceleratorModel::~BDSAcceleratorModel()
{
  // User feedback as deletion can take some time
  G4cout << "BDSAcceleratorModel> Deleting model" << G4endl;
  
  delete worldPV;
  delete worldLV;
  delete worldSolid;
  
  delete tunnelBeamline;
  delete placementBeamline;

  mainBeamlineSet.DeleteContents();
  
  for (auto& bl : extraBeamlines)
    {bl.second.DeleteContents();}
  
  delete BDSAcceleratorComponentRegistry::Instance();
  delete BDSPhysicalVolumeInfoRegistry::Instance();

  for (auto f : fields)
    {delete f;}
  for (auto r : regions)
    {delete r.second;}
  for (auto a : apertures)
    {delete a.second;}
  for (auto c : cuts)
    {delete c.second;}

  for (auto vr : volumeRegistries)
    {delete vr.second;}

  G4cout << "BDSAcceleratorModel> Deletion complete" << G4endl;

  instance = nullptr;
}

void BDSAcceleratorModel::RegisterBeamlineSetMain(const BDSBeamlineSet& setIn)
{
  mainBeamlineSet = setIn;
  MapBeamlineSet(setIn);
}

void BDSAcceleratorModel::RegisterBeamlineSetExtra(G4String              name,
						   const BDSBeamlineSet& setIn)
{
  auto search = extraBeamlines.find(name);
  if (search != extraBeamlines.end()) // already exists!
    {search->second.DeleteContents();} // delete pre-existing one for replacement
  extraBeamlines[name] = setIn;
  MapBeamlineSet(setIn);
}

const BDSBeamlineSet& BDSAcceleratorModel::BeamlineSet(G4String name) const
{
  if (name == "main")
    {return mainBeamlineSet;}
  
  const auto search = extraBeamlines.find(name);
  if (search == extraBeamlines.end())
    {G4cerr << __METHOD_NAME__ << "No such beam line set \"" << name << "\"" << G4endl; exit(1);}
  else
    {return search->second;}
}

void BDSAcceleratorModel::RegisterRegion(G4Region* region, G4ProductionCuts* cut)
{
  G4String name = region->GetName();
  regions[name] = region;
  cuts[name]    = cut;
}

void BDSAcceleratorModel::RegisterApertures(const std::map<G4String, BDSApertureInfo*>& aperturesIn)
{
  apertures.insert(aperturesIn.begin(), aperturesIn.end());
}

BDSApertureInfo* BDSAcceleratorModel::Aperture(G4String name) const
{
  auto result = apertures.find(name);
  if (result != apertures.end())
    {return result->second;}
  else
    {
      G4cerr << "Invalid aperture name \"" << name << "\"" << G4endl;
      exit(1);
    }
}

G4Region* BDSAcceleratorModel::Region(G4String name) const
{
  auto result = regions.find(name);
  if (result != regions.end())
    {return result->second;}
  else
    {
      G4cerr << "Invalid region name \"" << name << "\"" << G4endl;
      G4cout << "Available regions are: " << G4endl;
      for (const auto& r : regions)
	{G4cout << r.first << " ";}
      G4cout << G4endl;
      exit(1);
    }
}

std::set<G4LogicalVolume*>* BDSAcceleratorModel::VolumeSet(G4String name)
{
  if (volumeRegistries.find(name) == volumeRegistries.end()) // no such registry -> reate one
    {volumeRegistries[name] = new std::set<G4LogicalVolume*>();}
  return volumeRegistries[name];
}

G4bool BDSAcceleratorModel::VolumeInSet(G4LogicalVolume* volume,
					G4String registryName)
{
  if (volumeRegistries.find(registryName) == volumeRegistries.end())
    {return false;} // no such registry
  else
    {
      auto registry = volumeRegistries[registryName];
      return registry->find(volume) != registry->end();
    }
}

BDSBeamline* BDSAcceleratorModel::CorrespondingMassWorldBeamline(BDSBeamline* bl) const
{
  auto result = clToMassWorldMap.find(bl);
  if (result != clToMassWorldMap.end())
    {return result->second;}
  else
    {return nullptr;}
}

void BDSAcceleratorModel::MapBeamlineSet(const BDSBeamlineSet& setIn)
{
  clToMassWorldMap[setIn.massWorld]              = setIn.massWorld;
  clToMassWorldMap[setIn.curvilinearWorld]       = setIn.massWorld;
  clToMassWorldMap[setIn.curvilinearBridgeWorld] = setIn.massWorld;

  massWorldMapTF[setIn.massWorld]              = true;
  massWorldMapTF[setIn.curvilinearWorld]       = false;
  massWorldMapTF[setIn.curvilinearBridgeWorld] = false;
  if (setIn.endPieces) // optional 'beam line'
    {massWorldMapTF[setIn.endPieces] = false;}
}

G4bool BDSAcceleratorModel::BeamlineIsMassWorld(BDSBeamline* bl) const
{
  return BDS::MapGetWithDefault(massWorldMapTF, bl, false);
}

void BDSAcceleratorModel::MassWorldBeamlineAndIndex(BDSBeamline*& bl,
						    G4int& index) const
{
  if (!BeamlineIsMassWorld(bl))
    {
      // update pointer to mass world beam line
      bl = clToMassWorldMap.at(bl);
      // for both the curvilinear and curvilinear bridge worlds, there is one
      // extra element, so we decrement the index by one here to get the index
      // in the mass world.
      if (index > 0) // shouldn't happen, but we shouldn't make it < 0
	{index--;}
    } // else leave unchanged..
}
