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
#ifndef BDSTRACKINGACTION_H
#define BDSTRACKINGACTION_H

#include "globals.hh" // geant4 types / globals
#include "G4UserTrackingAction.hh"

class BDSEventAction;
class G4Track;

/**
 * @brief Action to decide whether or not to store trajectory information.
 */

class BDSTrackingAction: public G4UserTrackingAction
{
public:
  BDSTrackingAction(G4bool batchMode,
		    G4bool storeTrajectoryIn,
		    G4bool suppressTransportationStepsIn,
		    BDSEventAction* eventActionIn,
		    G4int  verboseEventNumberLevelIn,
		    G4int  verboseEventNumberIn,
		    G4int  verboseEventNumberContinueForIn,
		    G4bool verboseEventNumberPrimaryOnlyIn);
  
  virtual ~BDSTrackingAction(){;}

  /// Used to decide whether or not to store trajectories.
  virtual void PreUserTrackingAction(const G4Track* track);

  /// Detect whether track is a primary and if so whether it ended in a collimator.
  virtual void PostUserTrackingAction(const G4Track* track);

private:
  /// No default constructor required.
  BDSTrackingAction() = delete;
  
  /// Whether we're using the visualiser - in which case always store
  /// trajectories for visualisation.
  const G4bool interactive;

  /// Flag from global constants to control storing all trajectories. Cache
  /// this from global constants at the beginning as it doesn't change.
  const G4bool storeTrajectory;

  /// Cache of whether to suppress transportation steps in each trajectory.
  const G4bool suppressTransportationSteps;

  /// Cache of event action to communicate whether a primary stopped in a collimator or not.
  BDSEventAction* eventAction;

  G4int  verboseEventNumberLevel;
  G4int  verboseEventNumber;
  G4int  verboseEventNumberStop;
  G4bool verboseEventNumberPrimaryOnly;
};

#endif
