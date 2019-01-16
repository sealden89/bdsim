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
#ifndef BDSPRIMARYGENERATORACTION_H
#define BDSPRIMARYGENERATORACTION_H

#include "BDSExtent.hh"

#include "globals.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

class BDSBunch;
class BDSIonDefinition;
class BDSOutputLoader;
class BDSParticleDefinition;
class BDSPTCOneTurnMap;
class G4Event;
class G4ParticleGun;

/**
 * @brief Generates primary particle vertices using BDSBunch.
 *
 */

class BDSPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:
  BDSPrimaryGeneratorAction(BDSBunch*              bunchIn,
			    BDSParticleDefinition* beamParticleIn);
  virtual ~BDSPrimaryGeneratorAction();
  
  virtual void GeneratePrimaries(G4Event*);

  /// Set the world extent that particle coordinates will be checked against.
  inline void SetWorldExtent(const BDSExtent worldExtentIn) {worldExtent = worldExtentIn;}
  /// Register a PTC map instance used in the teleporter which this
  /// class will set initial (first turn) primary coordinates for.
  void RegisterPTCOneTurnMap(BDSPTCOneTurnMap* otmIn) {oneTurnMap = otmIn;}

private:
  /// Beam particle.
  BDSParticleDefinition* beamParticle;
  const BDSIonDefinition* ionDefinition; ///< Ion definition. This doesn't own it.
  
  /// Pointer a to G4 service class.
  G4ParticleGun*   particleGun;	  
  /// Pointer to the particle distribution generator.
  BDSBunch*        bunch;

  /// Cache of whether to write seed state as ASCII per event.
  G4bool writeASCIISeedState;

  /// Optional output handler for restoring seed state.
  BDSOutputLoader* recreateFile;

  /// Whether to load seed state at start of event from rootevent file.
  G4bool recreate;

  /// The offset in the file to read events from when setting the seed.
  G4int  eventOffset;

  /// Whether to use the ascii seed state each time.
  G4bool useASCIISeedState;

  /// World extent that particle coordinates are checked against to ensure they're inside it.
  BDSExtent worldExtent;

  G4bool ionPrimary; ///< The primary particle will be an ion.

  /// Can only get a G4ParticleDefinition for an ion when primary generator is called
  /// so cache the first time. This is the flag of that cache.
  G4bool ionCached; 
  
  G4double particleCharge; ///< Charge that will replace default ion charge.

  /// Cached OTM for setting first turn primary coords.
  BDSPTCOneTurnMap* oneTurnMap;
};

#endif
