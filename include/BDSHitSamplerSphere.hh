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
#ifndef BDSHITSAMPLERSPHERE_H
#define BDSHITSAMPLERSPHERE_H
#include "BDSParticleCoordsSpherical.hh"

#include "globals.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

/**
 * @brief The information recorded from a particle impacting a sampler.
 *
 * Everything public for simplicity of the class.
 */

class BDSHitSamplerSphere: public G4VHit
{
public:
  BDSHitSamplerSphere() = delete; ///< No default constructor.
  BDSHitSamplerSphere(G4int samplerIDIn,
		      const BDSParticleCoordsSpherical& coordsIn,
		      G4double totalEnergyIn,
		      G4double weightIn,
		      G4double SIn,
		      G4double momentumIn,
		      G4double massIn,
		      G4double chargeIn,
		      G4double rigidityIn,
		      G4int    pdgIDIn,
		      G4int    parentIDIn,
		      G4int    trackIDIn,
		      G4int    turnsTakenIn,
		      G4int    beamlineIndexIn,
		      G4int    nElectronsIn = 0);
  
  /// Note this should not be inline when we use a G4Allocator.
  virtual ~BDSHitSamplerSphere();
  
  inline void* operator new(size_t);
  inline void operator delete(void *aHit);

  G4int samplerID;
  BDSParticleCoordsSpherical coords;
  G4double totalEnergy;
  G4double weight;
  G4double S;
  G4double momentum;
  G4double mass;
  G4double charge; ///< Double as g4 uses charge as a double.
  G4double rigidity;
  G4int pdgID;
  G4int parentID;
  G4int trackID;
  G4int turnsTaken;
  G4int beamlineIndex;
  G4int nElectrons; ///< Can only get this at inspection time so include here.
};

typedef G4THitsCollection<BDSHitSamplerSphere> BDSHitsCollectionSamplerSphere;
extern G4Allocator<BDSHitSamplerSphere> BDSAllocatorSamplerSphere;

inline void* BDSHitSamplerSphere::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSAllocatorSamplerSphere.MallocSingle();
  return aHit;
}

inline void BDSHitSamplerSphere::operator delete(void *aHit)
{
  BDSAllocatorSamplerSphere.FreeSingle((BDSHitSamplerSphere*) aHit);
}

#endif
