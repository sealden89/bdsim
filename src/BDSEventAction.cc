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
#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSEventAction.hh"
#include "BDSEventInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutput.hh"
#include "BDSSamplerHit.hh"
#include "BDSSamplerSD.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSamplerInfo.hh"
#include "BDSSDManager.hh"
#include "BDSStackingAction.hh"
#include "BDSTerminatorSD.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryPrimary.hh"
#include "BDSUtilities.hh"
#include "BDSVolumeExitSD.hh"

#include "globals.hh"                  // geant4 types / globals
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4Run.hh"
#include "G4SDManager.hh"
#include "G4TrajectoryContainer.hh"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <map>
#include <string>
#include <vector>

using namespace std::chrono;

G4bool FireLaserCompton;  // bool to ensure that Laserwire can only occur once in an event

namespace {
  // Function to recursively connect t
  void connectTraj(std::map<BDSTrajectory*, bool> &interestingTraj, BDSTrajectory* t)
  {
    BDSTrajectory *t2 = t->GetParent();
    if (t2)
      {
	interestingTraj[t2] = true;
	connectTraj(interestingTraj, t2);
      }
    else
      {return;}
  }
}

BDSEventAction::BDSEventAction(BDSOutput* outputIn):
  output(outputIn),
  samplerCollID_plane(-1),
  samplerCollID_cylin(-1),
  energyCounterCollID(-1),
  tunnelEnergyCounterCollID(-1),
  worldEnergyCounterCollID(-1),
  worldExitCollID(-1),
  startTime(0),
  stopTime(0),
  starts(0),
  stops(0),
  seedStateAtStart(""),
  eventInfo(nullptr)
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  verboseEvent              = globals->VerboseEvent();
  verboseEventNumber        = globals->VerboseEventNumber();
  isBatch                   = globals->Batch();
  storeTrajectory           = globals->StoreTrajectory();
  trajectoryEnergyThreshold = globals->StoreTrajectoryEnergyThreshold();
  trajectoryCutZ            = globals->TrajCutGTZ();
  trajectoryCutR            = globals->TrajCutLTR();
  trajConnect               = globals->TrajConnect();
  particleToStore           = globals->StoreTrajectoryParticle();
  particleIDToStore         = globals->StoreTrajectoryParticleID(); 
  depth                     = globals->StoreTrajectoryDepth();
  samplerIDsToStore         = globals->StoreTrajectorySamplerIDs();
  sRangeToStore             = globals->StoreTrajectoryELossSRange();


  printModulo = globals->PrintModuloEvents();

  // particleID to store in integer vector
  std::stringstream iss(particleIDToStore);
  G4int i;
  while (iss >> i)
    particleIDIntToStore.push_back(i);
}

BDSEventAction::~BDSEventAction()
{}

void BDSEventAction::BeginOfEventAction(const G4Event* evt)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "processing begin of event action" << G4endl;
#endif
  BDSStackingAction::energyKilled = 0;
  primaryAbsorbedInCollimator = false; // reset flag
  
  // set samplers for trajectory (cannot be done in contructor)
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  samplerIDsToStore           = globals->StoreTrajectorySamplerIDs();
  
  // reset navigators to ensure no mis-navigating and that events are truly independent
  BDSAuxiliaryNavigator::ResetNavigatorStates();
  
  // update reference to event info
  eventInfo = static_cast<BDSEventInfo*>(evt->GetUserInformation());

  // number feedback
  G4int event_number = evt->GetEventID();
  BDSTerminatorSD::eventNumber = event_number; // update static member of terminator
  eventInfo->SetIndex(event_number);
  if (event_number%printModulo == 0)
    {G4cout << "---> Begin of event: " << event_number << G4endl;}
  if(verboseEvent)
    {G4cout << __METHOD_NAME__ << "event #" << event_number << G4endl;}

  // cache hit collection IDs for quicker access
  if(samplerCollID_plane < 0)
    { // if one is -1 then all need initialised.
      G4SDManager*  g4SDMan  = G4SDManager::GetSDMpointer();
      BDSSDManager* bdsSDMan = BDSSDManager::Instance();
      samplerCollID_plane       = g4SDMan->GetCollectionID(bdsSDMan->GetSamplerPlaneSD()->GetName());
      samplerCollID_cylin       = g4SDMan->GetCollectionID(bdsSDMan->GetSamplerCylinderSD()->GetName());
      energyCounterCollID       = g4SDMan->GetCollectionID(bdsSDMan->GetEnergyCounterSD()->GetName());
      tunnelEnergyCounterCollID = g4SDMan->GetCollectionID(bdsSDMan->GetEnergyCounterTunnelSD()->GetName());
      worldEnergyCounterCollID  = g4SDMan->GetCollectionID(bdsSDMan->GetEnergyCounterWorldSD()->GetName());
      worldExitCollID           = g4SDMan->GetCollectionID(bdsSDMan->GetWorldExitSD()->GetName());
    }
  FireLaserCompton=true;

  // get the current time - last thing before we hand off to geant4
  startTime = time(nullptr);
  eventInfo->SetStartTime(startTime);
  eventInfo->SetStopTime(startTime); // initialise to duration of 0

  milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  starts = (G4double)ms.count()/1000.0;
}

void BDSEventAction::EndOfEventAction(const G4Event* evt)
{
  // Get event number information
  G4int event_number = evt->GetEventID();
  if(verboseEvent || verboseEventNumber == event_number)
    {G4cout << __METHOD_NAME__ << "processing end of event"<<G4endl;}
  eventInfo->SetIndex(event_number);

  // Record if event was aborted - ie whether it's useable for analyses.
  eventInfo->SetAborted(evt->IsAborted());
  
  // Get the current time
  stopTime = time(nullptr);
  eventInfo->SetStopTime(stopTime);

  // Timing information
  milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  stops = (G4double)ms.count()/1000.0;
  eventInfo->SetDuration(G4float(stops - starts));

  G4double memoryUsedMb = BDS::GetMemoryUsage();
  eventInfo->SetMemoryUsage(memoryUsedMb);

  // cache if primary was absorbed in a collimator
  eventInfo->SetPrimaryAbsorbedInCollimator(primaryAbsorbedInCollimator);

  // Get the hits collection of this event - all hits from different SDs.
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();

  // samplers
  BDSSamplerHitsCollection* SampHC = nullptr;
  if(samplerCollID_plane >= 0)
    {SampHC = (BDSSamplerHitsCollection*)(evt->GetHCofThisEvent()->GetHC(samplerCollID_plane));}
  
  BDSSamplerHitsCollection* hitsCylinder = nullptr;
  if(samplerCollID_cylin >= 0)
    {hitsCylinder = (BDSSamplerHitsCollection*)(HCE->GetHC(samplerCollID_cylin));}

  // energy deposition collections - eloss, tunnel hits
  BDSEnergyCounterHitsCollection* energyCounterHits       = (BDSEnergyCounterHitsCollection*)(HCE->GetHC(energyCounterCollID));
  BDSEnergyCounterHitsCollection* tunnelEnergyCounterHits = (BDSEnergyCounterHitsCollection*)(HCE->GetHC(tunnelEnergyCounterCollID));
  BDSEnergyCounterHitsCollection* worldEnergyCounterHits  = (BDSEnergyCounterHitsCollection*)(HCE->GetHC(worldEnergyCounterCollID));

  // world exit hits
  BDSVolumeExitHitsCollection* worldExitHits = (BDSVolumeExitHitsCollection*)(HCE->GetHC(worldExitCollID));

  // primary hit something?
  // we infer this by seeing if there are any energy deposition hits at all - if there
  // are, the primary must have 'hit' something. possibly along step ionisation in vacuum
  // may fool this..
  if (energyCounterHits)
    {
      if (energyCounterHits->entries() > 0)
	{eventInfo->SetPrimaryHitMachine(true);}
    }
  if (tunnelEnergyCounterHits)
    {
      if (tunnelEnergyCounterHits->entries() > 0)
	{eventInfo->SetPrimaryHitMachine(true);}
    }
  // we don't check the world energy hits here because the hits could be from
  // intended transport through air in part of the machine (a gap).
  
  // primary hits and losses from
  const BDSTrajectoryPoint* primaryHit  = nullptr;
  const BDSTrajectoryPoint* primaryLoss = nullptr;
  G4TrajectoryContainer* trajCont = evt->GetTrajectoryContainer();
  if (trajCont)
    {
      BDSTrajectoryPrimary* primary = BDS::GetPrimaryTrajectory(trajCont);
      primaryHit  = primary->FirstHit();
      primaryLoss = primary->LastPoint();
    }

  // Save interesting trajectories
  std::map<BDSTrajectory*, bool> interestingTraj;

  if (storeTrajectory && trajCont)
  {

    TrajectoryVector* trajVec = trajCont->GetVector();

#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "trajectories ntrajectory=" << trajCont->size()
	   << " storeTrajectoryEnergyThreshold=" << trajectoryEnergyThreshold << G4endl;
#endif

    // build trackID map, depth map
    std::map<int, BDSTrajectory*> trackIDMap;
    std::map<BDSTrajectory*, int> depthMap;
    for (auto iT1 : *trajVec)
      {
	BDSTrajectory* traj = static_cast<BDSTrajectory*>(iT1);
	
	// fill track ID map
	trackIDMap.insert(std::pair<int, BDSTrajectory *>(traj->GetTrackID(), traj));

	// fill depth map
	if (traj->GetParentID() == 0) 
	  {depthMap.insert(std::pair<BDSTrajectory*, int>(traj, 0));}
	else
	  {depthMap.insert(std::pair<BDSTrajectory*, int>(traj, depthMap.at(trackIDMap.at(traj->GetParentID())) + 1));}
      }

    // fill parent pointer (TODO can this be merged with previous loop?)
    for (auto iT1 : *trajVec) 
      {
        BDSTrajectory* traj = static_cast<BDSTrajectory*>(iT1);	
        traj->SetParent(trackIDMap[iT1->GetParentID()]);
      }

    // loop over trajectories and determine if it should be stored
    for (auto iT1 : *trajVec)
      {
	BDSTrajectory* traj = static_cast<BDSTrajectory*>(iT1);
	G4int parentID = traj->GetParentID();
	
	// always store primaries
	if (parentID == 0)
	  {
	    interestingTraj.insert(std::pair<BDSTrajectory*, bool>(traj, true));
	    continue;
	  }
	
	// check on energy (if energy threshold is not negative)
	if (trajectoryEnergyThreshold >= 0 &&
	    traj->GetInitialKineticEnergy() > trajectoryEnergyThreshold)
	  {
	    interestingTraj.insert(std::pair<BDSTrajectory*, bool>(traj, true));
	    continue;
	  }
	
	// check on particle if not empty string

	if (!particleToStore.empty() || !particleIDToStore.empty())
	  {
	    G4String particleName  = traj->GetParticleName();
	    G4int particleID       = traj->GetPDGEncoding();
	    G4String particleIDStr = G4String(std::to_string(particleID));
	    std::size_t found1      = particleToStore.find(particleName);
	    bool        found2     = (std::find(particleIDIntToStore.begin(), particleIDIntToStore.end(),particleID) 
				      != particleIDIntToStore.end());
	    if ((found1 != std::string::npos) || found2)
	      {
		interestingTraj.insert(std::pair<BDSTrajectory *, bool>(traj, true));
		continue;
	      }
	  }
	
	// check on trajectory tree depth (depth = 0 means only primaries)
	if (depthMap[traj] <= depth)
	  {
	    interestingTraj.insert(std::pair<BDSTrajectory*, bool>(traj, true));
	    continue;
	  }
	
	// check on coordinates (and TODO momentum)
	// clear out trajectories that don't reach point TrajCutGTZ or greater than TrajCutLTR
	BDSTrajectoryPoint* trajEndPoint = static_cast<BDSTrajectoryPoint*>(traj->GetPoint(traj->GetPointEntries() - 1));
	G4bool greaterThanZInteresting = trajEndPoint->GetPosition().z() > trajectoryCutZ;
	G4bool withinRInteresting      = trajEndPoint->PostPosR() < trajectoryCutR;
	if (greaterThanZInteresting || withinRInteresting)
	  {
	    interestingTraj.insert(std::pair<BDSTrajectory*, bool>(traj, true));
	    continue;
	  }

	// if not interesting store false
	interestingTraj.insert(std::pair<BDSTrajectory*, bool>(traj, false));
      }
    
    // loop over energy hits to connect trajectories
    if(sRangeToStore.size() != 0)
      {
	G4int n_hit = energyCounterHits->entries();
	BDSEnergyCounterHit *hit;
	for(G4int i=0;i<n_hit;i++)
	  {
	    hit = (*energyCounterHits)[i];
	    // G4cout << hit->GetSHit() << " " << hit->GetTrackID() << G4endl;
	    double dS = hit->GetSHit();
	    for(auto v = sRangeToStore.begin(); v != sRangeToStore.end(); ++v) 
	      {		
		if ( dS >= (*v).first && dS <= (*v).second) 
		  {
		    interestingTraj[trackIDMap[hit->GetTrackID()]] = true;
		    break;
		  }
	      }
	  }
      }
    
    // loop over samplers to connect trajectories
    if(samplerIDsToStore.size() != 0)
      {
	G4int n_hit = SampHC->entries();
	for(G4int i=0;i<n_hit;i++)
	  {
	    G4int samplerIndex = (*SampHC)[i]->samplerID;
	    BDSSamplerInfo info = BDSSamplerRegistry::Instance()->GetInfo(samplerIndex);
	    // G4cout << i << " " << info.Name() << " " << info.UniqueName() << " " << info.SPosition() << G4endl;
	    if(std::find(samplerIDsToStore.begin(), samplerIDsToStore.end(),samplerIndex) != samplerIDsToStore.end())
	      {
		interestingTraj[trackIDMap[(*SampHC)[i]->trackID]] = true;
	      }
	  }
      }
    
    // Connect trajectory graphs
    if (trajConnect && trackIDMap.size() > 1)
      {
	for (auto i : interestingTraj)
	  if(i.second) 
	    {connectTraj(interestingTraj, i.first);}
      }
  }
  
  // Output interesting trajectories
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "storing trajectories nInterestingTrajectory=" << interestingTraj.size() << G4endl;
#endif
  
  output->FillEvent(eventInfo,
		    evt->GetPrimaryVertex(),
		    SampHC,
		    hitsCylinder,
		    energyCounterHits,
		    tunnelEnergyCounterHits,
		    worldEnergyCounterHits,
		    worldExitHits,
		    primaryHit,
		    primaryLoss,
		    interestingTraj,
		    BDSGlobalConstants::Instance()->TurnsTaken());
  
  // if events per ntuples not set (default 0) - only write out at end
  G4int evntsPerNtuple = BDSGlobalConstants::Instance()->NumberOfEventsPerNtuple();
  if (evntsPerNtuple>0 && (event_number+1)%evntsPerNtuple == 0)
    {
      // note the timing information will be wrong here as the run hasn't finished but
      // the file is bridged. There's no good way around this just now as this class
      // can't access the timing information stored in BDSRunAction
      output->CloseAndOpenNewFile();
    }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "end of event action done"<<G4endl;
#endif
}
