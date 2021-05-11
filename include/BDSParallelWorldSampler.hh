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
#ifndef BDSPARALLELWORLDSAMPER_H
#define BDSPARALLELWORLDSAMPER_H

#include "BDSBeamPipeType.hh"

#include "globals.hh"
#include "G4String.hh"
#include "G4VUserParallelWorld.hh"

#include <vector>

class G4VisAttributes;
class G4VPhysicalVolume;
class BDSBeamlineElement;
class BDSSampler;

/**
 * @brief A parallel world for sampler planes.
 *
 * @author Laurie Nevay
 */

class BDSParallelWorldSampler: public G4VUserParallelWorld
{
public:
  explicit BDSParallelWorldSampler(const G4String& name);
  virtual ~BDSParallelWorldSampler();

  /// Construct the required parallel world geometry. This must
  /// overload the pure virtual method in G4VUserParallelWorld.
  virtual void Construct();

  /// @{ Accessor.
  BDSSampler* GeneralPlane() const {return generalPlane;}
  G4LogicalVolume* WorldLV() const {return samplerWorldLV;}
  /// @}
  
  /// Place a sampler from a single element.
  void Place(const BDSBeamlineElement* element,
	     G4double                  samplerRadius);

private:
  /// No default constructor.
  BDSParallelWorldSampler() = delete;
  
  /// Cache of the placements to clean up at the end.
  std::vector<G4VPhysicalVolume*> placements;

  /// Just the input part of the world name.
  G4String suffix;

  /// Visualisation attributes for the sampler world.
  G4VisAttributes* samplerWorldVis;

  /// General single sampler we use for plane samplers.
  BDSSampler* generalPlane;
  
  G4LogicalVolume* samplerWorldLV;
};

#endif
