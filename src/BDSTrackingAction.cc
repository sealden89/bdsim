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
#include "BDSDebug.hh"
#include "BDSEventAction.hh"
#include "BDSGlobalConstants.hh"
#include "BDSTrackingAction.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryPrimary.hh"

#include "globals.hh" // geant4 types / globals
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"

#include <set>

class G4LogicalVolume;

BDSTrackingAction::BDSTrackingAction(G4bool batchMode,
				     G4bool storeTrajectoryIn,
				     G4bool suppressTransportationStepsIn,
				     BDSEventAction* eventActionIn,
				     G4int  verboseEventNumberLevelIn,
				     G4int  verboseEventNumberIn,
				     G4int  verboseEventNumberContinueForIn,
				     G4bool verboseEventNumberPrimaryOnlyIn):
  interactive(!batchMode),
  storeTrajectory(storeTrajectoryIn),
  suppressTransportationSteps(suppressTransportationStepsIn),
  eventAction(eventActionIn),
  verboseEventNumberLevel(verboseEventNumberLevelIn),
  verboseEventNumber(verboseEventNumberIn),
  verboseEventNumberStop(verboseEventNumberIn + verboseEventNumberContinueForIn),
  verboseEventNumberPrimaryOnly(verboseEventNumberPrimaryOnlyIn)
{
  if (verboseEventNumberContinueForIn < 1)
    {verboseEventNumberStop = verboseEventNumberIn + 1;}
}

void BDSTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  G4int eventIndex = eventAction->CurrentEventIndex();
  if (eventIndex == verboseEventNumber && !verboseEventNumberPrimaryOnly)
    {fpTrackingManager->GetSteppingManager()->SetVerboseLevel(verboseEventNumberLevel);}
  else if (eventIndex > verboseEventNumber && !verboseEventNumberPrimaryOnly && eventIndex < verboseEventNumberStop)
    {fpTrackingManager->GetSteppingManager()->SetVerboseLevel(verboseEventNumberLevel);}
  
  // if it's a primary track then we always store something
  if (track->GetParentID() == 0)
    {
      if (eventIndex == verboseEventNumber && verboseEventNumberPrimaryOnly)
	{fpTrackingManager->GetSteppingManager()->SetVerboseLevel(verboseEventNumberLevel);}
      else if (eventIndex > verboseEventNumber && verboseEventNumberPrimaryOnly && eventIndex < verboseEventNumberStop)
	{fpTrackingManager->GetSteppingManager()->SetVerboseLevel(verboseEventNumberLevel);}
      
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
  // turn off verbosity always as we selectiely turn it on in the start tracking option
  fpTrackingManager->GetSteppingManager()->SetVerboseLevel(0);
  
#ifdef BDSDEBUG
  G4int trackID = track->GetTrackID();
  if (trackID < 100)
    {// limit range of debug output
      auto status = track->GetTrackStatus();
      G4String name;
      switch (status)
	{
	case G4TrackStatus::fAlive:
	  {name = "fAlive"; break;}
	case G4TrackStatus::fStopButAlive:
	  {name = "fStopButAlive"; break;}
	case G4TrackStatus::fKillTrackAndSecondaries:
	  {name = "fKillTrackAndSecondaries"; break;}
	case G4TrackStatus::fStopAndKill:
	  {name = "fStopAndKill"; break;}
	default:
	  {name = "other"; break;}
	}  
      G4cout << "track ID " << trackID << " status " << name << G4endl;
    }
#endif
  if (track->GetParentID() == 0)
    {
      G4LogicalVolume* lv = track->GetVolume()->GetLogicalVolume();
      std::set<G4LogicalVolume*>* collimators = BDSAcceleratorModel::Instance()->VolumeSet("collimators");
      if (collimators->find(lv) != collimators->end())
	{eventAction->SetPrimaryAbsorbedInCollimator(true);}
    }
}
