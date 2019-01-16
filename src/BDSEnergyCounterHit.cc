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
#include "BDSEnergyCounterHit.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Allocator.hh"

G4Allocator<BDSEnergyCounterHit> BDSEnergyCounterHitAllocator;

BDSEnergyCounterHit::BDSEnergyCounterHit(const G4int&    nCopyIn,
					 const G4double& energyIn,
					 const G4double& preStepKineticEnergyIn,
					 const G4double& XIn, 
					 const G4double& YIn, 
					 const G4double& ZIn,
					 const G4double& sBeforeIn,
					 const G4double& sAfterIn,
					 const G4double& sHitIn,
					 const G4double& xIn,
					 const G4double& yIn,
					 const G4double& zIn,
					 const G4double& globalTimeIn,
					 const G4int&    partIDIn,
					 const G4int&    trackIDIn,
					 const G4int&    parentIDIn,
					 const G4double& weightIn, 
					 const G4int&    turnsTakenIn,
					 const G4int&    eventNoIn,
					 const G4double& stepLengthIn,
					 const G4int&    beamlineIndexIn):
  copyNumber(nCopyIn),
  energy(energyIn),
  preStepKineticEnergy(preStepKineticEnergyIn),
  X(XIn),
  Y(YIn),
  Z(ZIn),
  sBefore(sBeforeIn),
  sAfter(sAfterIn),
  sHit(sHitIn),
  x(xIn),
  y(yIn),
  z(zIn),
  globalTime(globalTimeIn),
  partID(partIDIn),
  trackID(trackIDIn),
  parentID(parentIDIn),
  weight(weightIn),
  turnsTaken(turnsTakenIn),
  eventNo(eventNoIn),
  stepLength(stepLengthIn),
  beamlineIndex(beamlineIndexIn)
{;}

BDSEnergyCounterHit::~BDSEnergyCounterHit()
{;}
