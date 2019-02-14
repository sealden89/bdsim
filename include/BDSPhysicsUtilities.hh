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
#ifndef BDSPHYSICSUTILITIES_H
#define BDSPHYSICSUTILITIES_H

#include "globals.hh"

#include "parser/fastlist.h"
#include "parser/physicsbiasing.h"

class BDSParticleDefinition;
class G4GenericBiasingPhysics;
class G4VModularPhysicsList;

namespace BDS
{
  /// Detect whether we're using a Geant4 provided physics list or whether we'll use the
  /// BDSIM modular physics and construct it.
  G4VModularPhysicsList* BuildPhysics(const G4String& physicsList);

  /// Construct the design and beam particle definitions. Even if these are the same, unique
  /// objects are created for and must be deleted elsewhere. Two pointers are passed by
  /// reference that will be updated with the allocated objects. The Boolean by reference
  /// argument is to tell whether they definitions (although unique objects) define the same
  /// particle.
  void ConstructDesignAndBeamParticle(const GMAD::BeamBase& beamDefinition,
				      G4double ffact,
				      BDSParticleDefinition*& designParticle,
				      BDSParticleDefinition*& beamParticle,
				      G4bool& beamDifferentFromDesignParticle);
  
  /// Construct particle definition. Ensure that particle is instantiated
  /// from a Geant4 point of view.  'ffact' is typically 1 or -1 used to flip
  /// the sign of the rigidity for difference between convention and what's required.
  BDSParticleDefinition* ConstructParticleDefinition(G4String particleNameIn,
						     G4double totalEnergy,
						     G4double ffact = 1);

  /// Ensure required beam particle has been constructed for Geant4 purposes.
  void ConstructBeamParticleG4(G4String name);
  
  /// Construct the minimum particle set required (gamma, electron, positron,
  /// proton and anti-proton.
  void ConstructMinimumParticleSet();

  /// Print all the processes by name as registered to the primary particle type.
  /// Note, this should only be done after the physics lists are fully constructed.
  void PrintPrimaryParticleProcesses(const G4String& primaryParticleName);

  /// Print all constructed particle names. Note, this should only be done after the
  /// physics lists are fully constructed.
  void PrintDefinedParticles();
  
  G4GenericBiasingPhysics* BuildAndAttachBiasWrapper(const GMAD::FastList<GMAD::PhysicsBiasing>& biases);

#if G4VERSION_NUMBER > 1039
  /// Build the physics required for channelling to work correctly.
  G4VModularPhysicsList* ChannellingPhysicsComplete(const G4bool useEMD = false);
#endif

  /// Set the range cuts on a physics list. This is split into a separate function to allow it
  /// to be applied to physics lists both from BDSIM's modular phyiscs list and other sources
  /// with the same mechanism.
  void SetRangeCuts(G4VModularPhysicsList* physicsList);

  /// Check if the user has requested a changed energy validity range and set the appropriate
  /// variables in the G4ProductionCutsTable.
  void CheckAndSetEnergyValidityRange();
}

#endif
