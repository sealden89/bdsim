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
#ifndef BDSVOLUMEEXITHIT_H
#define BDSVOLUMEEXITHIT_H

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

class BDSVolumeExitHit;

typedef G4THitsCollection<BDSVolumeExitHit> BDSVolumeExitHitsCollection;
extern G4Allocator<BDSVolumeExitHit> BDSVolumeExitHitAllocator;

/**
 * @brief Information recorded for a step leaving a volume.
 *
 */

class BDSVolumeExitHit: public G4VHit
{
public:
  /// Default (in effect) constructor for energy counter hit. The intention (by a lack of
  /// setter methods is that all information should be provided as instantiation time for
  /// an instance of this class.
  BDSVolumeExitHit(G4double totalEnergyIn,
		   G4double postStepKineticEnergyIn,
		   G4double XIn,
		   G4double YIn,
		   G4double ZIn,
		   G4double TIn,
		   G4int    partIDIn,
		   G4int    trackIDIn,
		   G4int    parentIDIn,
		   G4double weightIn,
		   G4int    turnsTakenIn);
  
  virtual ~BDSVolumeExitHit(){;}
  
  inline void* operator new(size_t);
  inline void operator delete(void *aHit);

  inline G4double TotalEnergyWeighted() const {return weight * totalEnergy;}
  
  G4double totalEnergy;

  /// Unlike kinetic energy recorded elsewhere, this is from the post-step point
  /// rather than the pre-step point because we want the kinetic energy leaving
  /// the volume.
  G4double postStepKineticEnergy;
  
  /// @{ Global coordinate
  G4double X;
  G4double Y;
  G4double Z;
  G4double T;
  /// @}
  
  G4int    partID;
  G4int    trackID;
  G4int    parentID;
  G4double weight;
  G4int    turnsTaken;

private:
  /// Private default constructor (not implemented) as the constructor.
  BDSVolumeExitHit() = delete;
};

inline void* BDSVolumeExitHit::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSVolumeExitHitAllocator.MallocSingle();
  return aHit;
}

inline void BDSVolumeExitHit::operator delete(void *aHit)
{
 BDSVolumeExitHitAllocator.FreeSingle((BDSVolumeExitHit*) aHit);
}

#endif
