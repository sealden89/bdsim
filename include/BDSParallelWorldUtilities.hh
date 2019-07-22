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
#ifndef BDSPARALLELWORLDUTILITIES_H
#define BDSPARALLELWORLDUTILITIES_H

#include "BDSParallelWorldInfo.hh"
#include "BDSParallelWorldImportance.hh"
#include "G4GeometrySampler.hh"
#include "globals.hh"

#include <vector>

class G4ParallelWorldPhysics;
class G4VModularPhysicsList;
class G4VUserDetectorConstruction;
class G4VUserParallelWorld;

namespace BDS
{
  /// The number of curvilinear and sampler worlds required. This is not curvilinear +
  /// curvilinear bridge. This is over and above the one for the main beam line. Note,
  /// an extra beam line, if it exists, will require a curvilinear world, but not
  /// necessarily a sampler world.
  std::vector<BDSParallelWorldInfo> NumberOfExtraWorldsRequired();

  /// Construct the default and any extra parallel worlds required and register them
  /// to the main mass world argument. Returns the vector of worlds that required a
  /// physics process so that their boundaries are respected in tracking.
  std::vector<G4VUserParallelWorld*> ConstructAndRegisterParallelWorlds(G4VUserDetectorConstruction* massWorld,
									G4bool buildSamplerWorld);

  /// Construct the parallel physics process for each sampler world.
  std::vector<G4ParallelWorldPhysics*> ConstructParallelWorldPhysics(std::vector<G4VUserParallelWorld *> worlds);

  /// Register each parallel physics process to the main physics list.
  void RegisterSamplerPhysics(std::vector<G4ParallelWorldPhysics*> processes,
			      G4VModularPhysicsList* physicsList);

  /// Get store, and prepare importance sampling for importance geometry sampler
  void AddIStore(std::vector<G4VUserParallelWorld*> worlds);

  /// Create importance geometry sampler and register importance biasing with physics list
  void RegisterImportanceBiasing(std::vector<G4VUserParallelWorld*> worlds,
                                 G4VModularPhysicsList* physicsList);

  /// Get importance sampling world from list of all parallel worlds
  BDSParallelWorldImportance* GetImportanceSamplingWorld(std::vector<G4VUserParallelWorld*> worlds);

}

#endif
