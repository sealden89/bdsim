/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
class BDSOutputLoader;
class BDSPTCOneTurnMap;
class BDSROOTSamplerReader;
class G4Event;
class G4ParticleGun;

namespace GMAD
{
  class Beam;
}

#ifdef USE_HEPMC3
class BDSHepMC3Reader;
#endif

/**
 * @brief Generates primary particle vertices using BDSBunch.
 *
 */

class BDSPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:
  /// Bunch must have a valid particle definition (ie not nullptr).
  BDSPrimaryGeneratorAction(BDSBunch*         bunchIn,
			    const GMAD::Beam& beam);
  virtual ~BDSPrimaryGeneratorAction();

  /// Main interface for Geant4. Prepare primary(ies) for the event.
  virtual void GeneratePrimaries(G4Event*);

  /// Set the world extent that particle coordinates will be checked against.
  inline void SetWorldExtent(const BDSExtent worldExtentIn) {worldExtent = worldExtentIn;}
  /// Register a PTC map instance used in the teleporter which this
  /// class will set initial (first turn) primary coordinates for.
  void RegisterPTCOneTurnMap(BDSPTCOneTurnMap* otmIn) {oneTurnMap = otmIn;}
private:  
  
  G4ParticleGun* particleGun;     ///< Geant4 particle gun that creates single particles.
  BDSBunch* bunch;                ///< BDSIM particle generator.  
  G4bool writeASCIISeedState;     ///< Cache of whether to write seed state as ASCII per event.
  BDSOutputLoader* recreateFile;  ///< Optional output handler for restoring seed state. 
  G4bool   recreate;              ///< Whether to load seed state at start of event from rootevent file.
  G4int    eventOffset;           ///< The offset in the file to read events from when setting the seed.
  G4bool   useASCIISeedState;     ///< Whether to use the ascii seed state each time.
  G4bool   ionPrimary;            ///< The primary particle will be an ion.
  G4bool   useEventGeneratorFile; ///< Whether to use event generator file.
  G4bool   useSamplerLoader;      ///< Whether to use a sampler loader.
  
  /// World extent that particle coordinates are checked against to ensure they're inside it.
  BDSExtent worldExtent;

  /// Can only get a G4ParticleDefinition for an ion when primary generator is called
  /// so cache the first time. This is the flag of that cache.
  G4bool ionCached; 

  /// Cached OTM for setting first turn primary coords.
  BDSPTCOneTurnMap* oneTurnMap;

#ifdef USE_HEPMC3
  /// Event generator file loader.
  BDSHepMC3Reader* hepMC3Reader;
#endif
  BDSROOTSamplerReader* samplerReader;
};

#endif
