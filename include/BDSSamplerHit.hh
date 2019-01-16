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
#ifndef BDSSAMPLERHIT_H
#define BDSSAMPLERHIT_H

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

#include "BDSParticleCoordsFull.hh"

#include <ostream>

/**
 * @brief The information recorded from a particle impacting a sampler.
 *
 * Everything public for simplicity of the class.
 */

class BDSSamplerHit: public G4VHit
{
public:
  BDSSamplerHit(G4int samplerIDIn,
		const BDSParticleCoordsFull& coordsIn,
		G4double chargeIn,
		G4int    pdgIDIn,
		G4int    parentIDIn,
		G4int    trackIDIn,
		G4int    turnsTakenIn,
		G4int    beamlineIndexIn);
  
  virtual ~BDSSamplerHit(){;}
  
  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);

  G4int                 samplerID;
  BDSParticleCoordsFull coords;
  G4double              charge; // double as g4 uses it as a double
  G4int                 pdgID;
  G4int                 parentID;
  G4int                 trackID;
  G4int                 turnsTaken;
  G4int                 beamlineIndex;
  
private:
  BDSSamplerHit() = delete; ///< No default constructor.
};

typedef G4THitsCollection<BDSSamplerHit> BDSSamplerHitsCollection;
extern G4Allocator<BDSSamplerHit> BDSSamplerHitAllocator;

inline void* BDSSamplerHit::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSSamplerHitAllocator.MallocSingle();
  return aHit;
}

inline void BDSSamplerHit::operator delete(void *aHit)
{
  BDSSamplerHitAllocator.FreeSingle((BDSSamplerHit*) aHit);
}

#endif
