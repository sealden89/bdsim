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
#ifndef BDSPARTICLEDEFINITION_H
#define BDSPARTICLEDEFINITION_H

#include "globals.hh"

#include <ostream>

class G4ParticleDefinition;

class BDSIonDefinition;

/**
 * @brief Wrapper for particle definition.
 *
 * Used to calculate momentum and kinetic energy and associate any other information
 * required along with a G4ParticleDefinition instance.
 *
 * @author Laurie Nevay
 */

class BDSParticleDefinition
{
public:
  /// Constructor requires G4 particle definition as well as user specified
  /// total energy. From this the momentum and kinetic energy are calculated
  /// and checked to be valid - ie sufficient total energy. The optional ion
  /// definition may be supplied to override the charge of the ion. ffact is
  /// typically 1 or -1 to flip the rigidity for convention.
  BDSParticleDefinition(G4ParticleDefinition* particleIn,
			G4double              totalEnergyIn,
			G4double              ffact,
			BDSIonDefinition*     ionDefinitionIn = nullptr);

  /// Alternative constructor for when we don't have access to the particle table
  /// information. G4ParticleDefinition can be updated later. Developer
  /// responsibility to ensure this matches the contents of this class.
  /// ffact is typically 1 or -1 to flip the rigidity for convention.
  BDSParticleDefinition(G4String          nameIn,
			G4double          massIn,
			G4double          chargeIn,
			G4double          totalEnergyIn,
			G4double          ffact,
		       	BDSIonDefinition* ionDefinitionIn = nullptr);

  /// Copy constructor specified as we have to copy the ionDefinition if it exists.
  BDSParticleDefinition(const BDSParticleDefinition& other);
  
  ~BDSParticleDefinition();

  /// Update the G4 particle definition member. Developer responsibility to ensure
  /// this matches the contents of the class.
  void UpdateG4ParticleDefinition(G4ParticleDefinition* particleIn) {particle = particleIn;}

  /// @{ Accessor.
  inline G4ParticleDefinition* ParticleDefinition() const {return particle;}
  inline BDSIonDefinition*     IonDefinition()      const {return ionDefinition;}
  inline G4String Name()          const {return name;}
  inline G4double Mass()          const {return mass;}
  inline G4double Charge()        const {return charge;}
  inline G4double TotalEnergy()   const {return totalEnergy;}
  inline G4double KineticEnergy() const {return kineticEnergy;}
  inline G4double Momentum()      const {return momentum;}
  inline G4double Gamma()         const {return gamma;}
  inline G4double Beta()          const {return beta;}
  inline G4double BRho()          const {return brho;}
  inline G4bool   IsAnIon()       const {return ionDefinition != nullptr;}
  /// @}

  /// Output stream operator implementation.
  friend std::ostream& operator<< (std::ostream& out, BDSParticleDefinition const& def);
  
private:
  /// No default constructor.
  BDSParticleDefinition() = delete;

  /// Calculate and set momentum based on totalEnergy and mass.
  void CalculateMomentum();

  /// Calculate and set rigidity based on charge and momentum.
  void CalculateRigidity(const G4double& ffact);

  /// Calculate and set lorentz factors.
  void CalculateLorentzFactors();
  
  G4ParticleDefinition* particle;      ///< Does not own.
  BDSIonDefinition*     ionDefinition; ///< Optional ion definition. Does own.

  G4String name;           ///< Particle name.
  G4double mass;           ///< Particle mass.
  G4double charge;         ///< Particle charge.
  G4double totalEnergy;    ///< Particle total energy.
  G4double kineticEnergy;  ///< Particle kinetic energy.
  G4double momentum;       ///< Particle momentum.
  G4double gamma;          ///< Relativistic gamma.
  G4double beta;           ///< Relativistic beta.
  G4double brho;           ///< Particle rigidity.
};

#endif
