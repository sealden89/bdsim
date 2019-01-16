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
#include "BDSAcceleratorModel.hh"
#include "BDSEventAction.hh"
#include "BDSTrackingAction.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryPrimary.hh"

#include "globals.hh" // geant4 types / globals
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"

#include <set>

class G4LogicalVolume;

BDSTrackingAction::BDSTrackingAction(const G4bool& batchMode,
				     const G4bool& storeTrajectoryIn,
				     const G4bool& suppressTransportationStepsIn,
				     BDSEventAction* eventActionIn):
  interactive(!batchMode),
  storeTrajectory(storeTrajectoryIn),
  suppressTransportationSteps(suppressTransportationStepsIn),
  eventAction(eventActionIn)
{;}

void BDSTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  // if it's a primary track then we always store something
  if (track->GetParentID() == 0)
    {
      // only store the actual trajectory points if we explicitly want
      // trajectory points or we're using the visualiser.
      G4bool storePoints = storeTrajectory || interactive;
      auto traj = new BDSTrajectoryPrimary(track,
					   interactive,
					   suppressTransportationSteps,
					   storePoints);
      fpTrackingManager->SetStoreTrajectory(1);
      fpTrackingManager->SetTrajectory(traj);
    }
  else
    {
      // not a primary - only store if we want to or interactive
      if (storeTrajectory || interactive)
	{
	  auto traj = new BDSTrajectory(track,
					interactive,
					suppressTransportationSteps);
	  fpTrackingManager->SetStoreTrajectory(1);
	  fpTrackingManager->SetTrajectory(traj);
	}
      else // mark as don't store
	{fpTrackingManager->SetStoreTrajectory(0);}
    }
}

void BDSTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  if (track->GetParentID() == 0)
    {
      G4LogicalVolume* lv = track->GetVolume()->GetLogicalVolume();
      std::set<G4LogicalVolume*>* collimators = BDSAcceleratorModel::Instance()->VolumeSet("collimators");
      if (collimators->find(lv) != collimators->end())
	{eventAction->SetPrimaryAbsorbedInCollimator(true);}
    }
}
