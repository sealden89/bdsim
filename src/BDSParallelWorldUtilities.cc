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
#include "BDSParallelWorldCurvilinear.hh"
#include "BDSParallelWorldCurvilinearBridge.hh"
#include "BDSParallelWorldInfo.hh"
#include "BDSParallelWorldSampler.hh"
#include "BDSParallelWorldUtilities.hh"
#include "BDSParser.hh"
#include "BDSSamplerType.hh"

#include "parser/element.h"
#include "parser/fastlist.h"
#include "parser/placement.h"

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

std::vector<G4VUserParallelWorld*> BDS::ConstructAndRegisterParallelWorlds(G4VUserDetectorConstruction* massWorld)
{
  // standard worlds
  auto samplerWorld           = new BDSParallelWorldSampler("main");
  auto curvilinearWorld       = new BDSParallelWorldCurvilinear("main");
  auto curvilinearBridgeWorld = new BDSParallelWorldCurvilinearBridge("main");
  massWorld->RegisterParallelWorld(samplerWorld);
  massWorld->RegisterParallelWorld(curvilinearWorld);
  massWorld->RegisterParallelWorld(curvilinearBridgeWorld);

  // extra worlds
  std::vector<BDSParallelWorldInfo> worldInfos = BDS::NumberOfExtraWorldsRequired();

  // register of all created
  //std::map<G4String, G4VUserParallelWorld*> worlds;
  std::vector<G4VUserParallelWorld*> samplerWorlds;
  samplerWorlds.push_back(samplerWorld);
  
  for (auto info : worldInfos)
    {
      if (info.curvilinearWorld)
	{
	  auto cLWorld       = new BDSParallelWorldCurvilinear(info.sequenceName);
	  auto cLBridgeWorld = new BDSParallelWorldCurvilinearBridge(info.sequenceName);
	  //worlds[info.sequenceName + "_cl"] = cLWorld;
	  massWorld->RegisterParallelWorld(cLWorld);
	  //worlds[info.sequenceName + "_clb"] = cLBridgeWorld;
	  massWorld->RegisterParallelWorld(cLBridgeWorld);
	}
      if (info.samplerWorld)
	{
	  BDSParallelWorldSampler* sWorld = new BDSParallelWorldSampler(info.sequenceName);
	  //worlds[info.sequenceName + "_s"] = sWorld;
	  samplerWorlds.push_back(sWorld);
	  massWorld->RegisterParallelWorld(sWorld);
	}
    }

  return samplerWorlds;
}

std::vector<G4ParallelWorldPhysics*> BDS::ConstructSamplerParallelPhysics(std::vector<G4VUserParallelWorld*> worlds)
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
