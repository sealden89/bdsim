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
#ifndef BDSSDENERGYDEPOSITION_H
#define BDSSDENERGYDEPOSITION_H

#include "BDSHitEnergyDeposition.hh"
#include "BDSSensitiveDetector.hh"

class BDSAuxiliaryNavigator;

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;
class G4Track;

/**
 * @brief Generates BDSHitsEnergyDepositions from step information - uses curvilinear coords.
 *
 * This class interrogates a G4Step and generates an energy deposition hit if there was
 * a change in energy. This assigns the energy deposition to a point randomly (uniformly)
 * along the step.  It also uses a BDSAuxiliaryNavigator instance to use transforms from
 * the curvilinear parallel world for curvilinear coordinates.
 */

class BDSSDEnergyDeposition: public BDSSensitiveDetector
{
public:
  BDSSDEnergyDeposition(G4String name,
			G4bool   storeExtrasIn);
  virtual ~BDSSDEnergyDeposition();

  virtual void Initialize(G4HCofThisEvent* HCE);

  /// The standard interface here to process a step from Geant4. Record
  /// all the relevant coordinates here. Records the energy deposited along
  /// the step.
  virtual G4bool ProcessHits(G4Step* aStep,
			     G4TouchableHistory* th);

  /// An extra interface that can be used when artificially killing a track
  /// to add that kill action as energy deposition of the particle there.
  /// Note, this method always uses the total energy of the particle. There
  /// is no step here, so it's just the total energy of the particle.
  virtual G4bool ProcessHitsTrack(const G4Track* track,
				  G4TouchableHistory* th);

  /// Provide access to last hit.
  virtual G4VHit* last() const;
  
private:
  /// assignment and copy constructor not implemented nor used
  BDSSDEnergyDeposition& operator=(const BDSSDEnergyDeposition&);
  BDSSDEnergyDeposition(BDSSDEnergyDeposition&);
  BDSSDEnergyDeposition() = delete;
  
  G4bool   storeExtras;     ///< Whether to store extra information.
  G4String colName;         ///< Collection name.
  BDSHitsCollectionEnergyDeposition* hits;
  G4int    HCIDe;

  ///@{ Per hit variable.
  G4double energy;
  G4double preStepKineticEnergy;
  G4double weight;
  G4double X,Y,Z;      // Global coordinates.
  G4double x,y,z;      // Local coordinates.
  G4double sBefore;
  G4double sAfter;
  G4double globalTime; // Time since start of event.
  G4double stepLength;
  G4int    ptype;
  G4int    trackID;
  G4int    parentID;
  G4int    turnsTaken;
  ///@}

  /// Navigator for checking points in read out geometry
  BDSAuxiliaryNavigator* auxNavigator;
};

#endif

