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
#include "BDSVolumeExitHit.hh"

#include "G4Allocator.hh"

G4Allocator<BDSVolumeExitHit> BDSVolumeExitHitAllocator;

BDSVolumeExitHit::BDSVolumeExitHit(G4double totalEnergyIn,
				   G4double postStepKineticEnergyIn,
				   G4double XIn,
				   G4double YIn,
				   G4double ZIn,
				   G4double TIn,
				   G4int    partIDIn,
				   G4int    trackIDIn,
				   G4int    parentIDIn,
				   G4double weightIn,
				   G4int    turnsTakenIn):
  totalEnergy(totalEnergyIn),
  postStepKineticEnergy(postStepKineticEnergyIn),
  X(XIn),
  Y(YIn),
  Z(ZIn),
  T(TIn),
  partID(partIDIn),
  trackID(trackIDIn),
  parentID(parentIDIn),
  weight(weightIn),
  turnsTaken(turnsTakenIn)
{;}
