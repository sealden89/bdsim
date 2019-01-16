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
#include "BDSParticleCoordsFull.hh"
#include "BDSSamplerHit.hh"

#include "globals.hh"
#include "G4Allocator.hh"

G4Allocator<BDSSamplerHit> BDSSamplerHitAllocator;

BDSSamplerHit::BDSSamplerHit(G4int samplerIDIn,
			     const BDSParticleCoordsFull& coordsIn,
			     G4double chargeIn,
			     G4int    pdgIDIn,
			     G4int    parentIDIn,
			     G4int    trackIDIn,
			     G4int    turnsTakenIn,
			     G4int    beamlineIndexIn):
  samplerID(samplerIDIn),
  coords(coordsIn),
  charge(chargeIn),
  pdgID(pdgIDIn),
  parentID(parentIDIn),
  trackID(trackIDIn),
  turnsTaken(turnsTakenIn),
  beamlineIndex(beamlineIndexIn)
{;}
