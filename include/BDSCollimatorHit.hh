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
#ifndef BDSCOLLIMATORHIT_H
#define BDSCOLLIMATORHIT_H

#include "globals.hh"
#include "G4VHit.hh"
#include "G4ThreeVector.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

class BDSBeamline;
class BDSEnergyCounterHit;

/**
 * @brief Snapshot of information for particle passing through a collimator.
 *
 * Everything public for simplicity of the class.
 * 
 * @author Laurie Nevay
 */

class BDSCollimatorHit: public G4VHit
{
public:
  BDSCollimatorHit();
  BDSCollimatorHit(const BDSBeamline*   beamlineIn,
		   G4int                collimatorIndexIn,
		   const G4ThreeVector& preStepPositionIn,
		   const G4ThreeVector& preStepMomentumIn,
		   G4double             totalEnergyIn,
		   BDSEnergyCounterHit* energyDepositionHitIn);

  virtual ~BDSCollimatorHit(){;}

  inline void* operator new(size_t);
  inline void operator delete(void *aHit);

  const BDSBeamline* beamline;   ///< Which beam line the collimator is in.
  G4int         collimatorIndex; ///< Index of collimator the hit is in.
  G4ThreeVector preStepPosition; ///< Local pre step point (z from centre of object).
  G4ThreeVector preStepMomentum; ///< Local pre step point momentum.
  G4double      totalEnergy;     ///< Total energy of particle.

  /// Pointer to the externally created energy deposition hit for the step in the
  /// collimator this hit will correspond to.
  BDSEnergyCounterHit* energyDepositionHit;
};

typedef G4THitsCollection<BDSCollimatorHit> BDSCollimatorHitsCollection;
extern G4Allocator<BDSCollimatorHit> BDSCollimatorHitAllocator;

inline void* BDSCollimatorHit::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSCollimatorHitAllocator.MallocSingle();
  return aHit;
}

inline void BDSCollimatorHit::operator delete(void *aHit)
{
  BDSCollimatorHitAllocator.FreeSingle((BDSCollimatorHit*) aHit);
}

#endif
