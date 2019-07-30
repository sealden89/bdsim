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
#include "BDSAcceleratorModel.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParallelWorldCurvilinear.hh"
#include "BDSParallelWorldCurvilinearBridge.hh"
#include "BDSParallelWorldImportance.hh"
#include "BDSParallelWorldInfo.hh"
#include "BDSParallelWorldSampler.hh"
#include "BDSParallelWorldUtilities.hh"
#include "BDSParser.hh"
#include "BDSSamplerType.hh"
#include "BDSUtilities.hh"

#include "parser/element.h"
#include "parser/fastlist.h"
#include "parser/placement.h"

#include "G4GeometrySampler.hh"
#include "G4ImportanceBiasing.hh"
#include "G4IStore.hh"
#include "G4ParallelWorldPhysics.hh"
#include "G4VModularPhysicsList.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VUserParallelWorld.hh"

#include "globals.hh"

#include <map>
#include <vector>

std::vector<BDSParallelWorldInfo> BDS::NumberOfExtraWorldsRequired()
{
  std::vector<BDSParallelWorldInfo> worlds;
  
  const std::vector<GMAD::Placement> placements = BDSParser::Instance()->GetPlacements();
  for (const auto& pl : placements)
    {// loop over all placement definitions and see if any are beam line placements
      if (!pl.sequence.empty()) // it's a beam line placement
	{// curvilinear world required at least
	  G4bool samplerWorldRequired = false;
	  for (const auto& el : BDSParser::Instance()->GetSequence(pl.sequence))
	    {// do any elements require a sampler -> sampler world required
	      BDSSamplerType sType = BDS::DetermineSamplerType(el.samplerType);
	      if (sType != BDSSamplerType::none)
		{
		  samplerWorldRequired = true;
		  break; // no need to loop over rest of sequence
		}
	    }
	  BDSParallelWorldInfo info(pl.sequence, true, samplerWorldRequired);
	  worlds.push_back(info);
	}
    }
  return worlds;
}

std::vector<G4VUserParallelWorld*> BDS::ConstructAndRegisterParallelWorlds(G4VUserDetectorConstruction* massWorld,
									   G4bool buildSamplerWorld)
{
  BDSAcceleratorModel* acceleratorModel = BDSAcceleratorModel::Instance();

  // registry of all created worlds that require the physics process so
  // that their boundaries affect tracking
  std::vector<G4VUserParallelWorld*> worldsRequiringPhysics;

  // standard worlds
  if (buildSamplerWorld) // optional
    {
      auto samplerWorld = new BDSParallelWorldSampler("main");
      massWorld->RegisterParallelWorld(samplerWorld);
      acceleratorModel->RegisterParallelWorld(samplerWorld);
      worldsRequiringPhysics.push_back(dynamic_cast<G4VUserParallelWorld*>(samplerWorld));
    }

  auto curvilinearWorld       = new BDSParallelWorldCurvilinear("main");
  auto curvilinearBridgeWorld = new BDSParallelWorldCurvilinearBridge("main");
  massWorld->RegisterParallelWorld(curvilinearWorld);
  massWorld->RegisterParallelWorld(curvilinearBridgeWorld);

  // G4VUserDetectorConstruction doesn't delete parallel worlds so we should
  acceleratorModel->RegisterParallelWorld(curvilinearWorld);
  acceleratorModel->RegisterParallelWorld(curvilinearBridgeWorld);

  // extra worlds for additional beam line placements
  std::vector<BDSParallelWorldInfo> worldInfos = BDS::NumberOfExtraWorldsRequired();

  for (auto info : worldInfos)
    {
      if (info.curvilinearWorld)
	{
	  auto cLWorld       = new BDSParallelWorldCurvilinear(info.sequenceName);
	  auto cLBridgeWorld = new BDSParallelWorldCurvilinearBridge(info.sequenceName);
	  massWorld->RegisterParallelWorld(cLWorld);
	  massWorld->RegisterParallelWorld(cLBridgeWorld);
	  acceleratorModel->RegisterParallelWorld(cLWorld);
	  acceleratorModel->RegisterParallelWorld(cLBridgeWorld);
	}
      if (info.samplerWorld)
	{
	  BDSParallelWorldSampler* sWorld = new BDSParallelWorldSampler(info.sequenceName);
	  acceleratorModel->RegisterParallelWorld(sWorld); // register for deletion with bdsim
	  worldsRequiringPhysics.push_back(dynamic_cast<G4VUserParallelWorld*>(sWorld));
	  massWorld->RegisterParallelWorld(sWorld);
	}
    }

  // only create the importance parallel world if the file is specified
  if (BDSGlobalConstants::Instance()->UseImportanceSampling())
    {
      G4String importanceWorldGeometryFile = BDSGlobalConstants::Instance()->ImportanceWorldGeometryFile();
      G4String importanceVolumeMapFile     = BDSGlobalConstants::Instance()->ImportanceVolumeMapFile();
      BDSParallelWorldImportance* importanceWorld = new BDSParallelWorldImportance("main",
                                                                                   importanceWorldGeometryFile,
                                                                                   importanceVolumeMapFile);
      acceleratorModel->RegisterParallelWorld(importanceWorld);
      massWorld->RegisterParallelWorld(importanceWorld);
      worldsRequiringPhysics.push_back(dynamic_cast<G4VUserParallelWorld*>(importanceWorld));
    }

  return worldsRequiringPhysics;
}

std::vector<G4ParallelWorldPhysics*> BDS::ConstructParallelWorldPhysics(std::vector<G4VUserParallelWorld *> worlds)
{
  std::vector<G4ParallelWorldPhysics*> result;
  for (auto world : worlds)
    {result.push_back(new G4ParallelWorldPhysics(world->GetName()));}
  return result;
}

void BDS::RegisterSamplerPhysics(std::vector<G4ParallelWorldPhysics*> processes,
				 G4VModularPhysicsList* physicsList)
{
  for (auto process : processes)
    {physicsList->RegisterPhysics(process);}
}

void BDS::AddIStore(std::vector<G4VUserParallelWorld*> worlds)
  {
    BDSParallelWorldImportance* importanceWorld = BDS::GetImportanceSamplingWorld(worlds);
    //only add importance store if the world exists
    if (importanceWorld)
      {importanceWorld->AddIStore();}
    else
      {throw BDSException(__METHOD_NAME__, "Importance sampling world not found.");}
  }

void BDS::RegisterImportanceBiasing(std::vector<G4VUserParallelWorld*> worlds,
                    G4VModularPhysicsList* physList)
{
  BDSParallelWorldImportance* importanceWorld = BDS::GetImportanceSamplingWorld(worlds);

  // create world geometry sampler
  G4GeometrySampler* pgs = new G4GeometrySampler(importanceWorld->GetWorldVolume(), "neutron");
  pgs->SetParallel(true);
  physList->RegisterPhysics(new G4ImportanceBiasing(pgs,importanceWorld->GetName()));
}

BDSParallelWorldImportance* BDS::GetImportanceSamplingWorld(std::vector<G4VUserParallelWorld*> worlds)
{
  // get importance world
  G4String importanceWorldName = "importanceWorld_main";
  G4VUserParallelWorld* importanceWorld = nullptr;
  for (auto world : worlds)
    {
      if (std::strcmp(world->GetName(),importanceWorldName) == 0)
        {importanceWorld = world; break;}
    }
  BDSParallelWorldImportance* iworld = dynamic_cast<BDSParallelWorldImportance*>(importanceWorld);
  return iworld;
}
