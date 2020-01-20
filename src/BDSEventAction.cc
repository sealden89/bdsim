/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSEventAction.hh"
#include "BDSEventInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSHitEnergyDeposition.hh"
#include "BDSHitEnergyDepositionExtra.hh"
#include "BDSHitEnergyDepositionGlobal.hh"
#include "BDSHitSampler.hh"
#include "BDSHitThinThing.hh"
#include "BDSOutput.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSamplerInfo.hh"
#include "BDSSDApertureImpacts.hh"
#include "BDSSDCollimator.hh"
#include "BDSSDEnergyDeposition.hh"
#include "BDSSDEnergyDepositionGlobal.hh"
#include "BDSSDManager.hh"
#include "BDSSDSampler.hh"
#include "BDSSDTerminator.hh"
#include "BDSSDThinThing.hh"
#include "BDSSDVolumeExit.hh"
#include "BDSStackingAction.hh"
#include "BDSTrajectoriesToStore.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryFilter.hh"
#include "BDSTrajectoryPrimary.hh"
#include "BDSUtilities.hh"

#include "globals.hh"                  // geant4 types / globals
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4PropagatorInField.hh"
#include "G4Run.hh"
#include "G4SDManager.hh"
#include "G4StackManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4TrajectoryPoint.hh"
#include "G4TransportationManager.hh"

#include <algorithm>
#include <bitset>
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
    BDSTrajectory* t2 = t->GetParent();
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
  eCounterID(-1),
  eCounterFullID(-1),
  eCounterVacuumID(-1),
  eCounterTunnelID(-1),
  eCounterWorldID(-1),
  eCounterWorldContentsID(-1),
  worldExitCollID(-1),
  collimatorCollID(-1),
  apertureCollID(-1),
  thinThingCollID(-1),
  startTime(0),
  stopTime(0),
  starts(0),
  stops(0),
  cpuStartTime(std::clock_t()),
  primaryAbsorbedInCollimator(false),
  seedStateAtStart(""),
  currentEventIndex(0),
  eventInfo(nullptr)
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  verboseEventBDSIM         = globals->VerboseEventBDSIM();
  verboseEventStart         = globals->VerboseEventStart();
  verboseEventStop          = BDS::VerboseEventStop(verboseEventStart, globals->VerboseEventContinueFor());
  storeTrajectory           = globals->StoreTrajectory();
  storeTrajectoryAll        = globals->StoreTrajectoryAll();
  trajectoryFilterLogicAND  = globals->TrajectoryFilterLogicAND();
  trajectoryEnergyThreshold = globals->StoreTrajectoryEnergyThreshold();
  trajectoryCutZ            = globals->TrajCutGTZ();
  trajectoryCutR            = globals->TrajCutLTR();
  trajConnect               = globals->TrajConnect();
  trajParticleNameToStore   = globals->StoreTrajectoryParticle();
  trajParticleIDToStore     = globals->StoreTrajectoryParticleID();
  trajDepth                 = globals->StoreTrajectoryDepth();
  trajSRangeToStore         = globals->StoreTrajectoryELossSRange();
  trajFiltersSet            = globals->TrajectoryFiltersSet();
  printModulo               = globals->PrintModuloEvents();

  // particleID to store in integer vector
  std::stringstream iss(trajParticleIDToStore);
  G4int i;
  while (iss >> i)
    {trajParticleIDIntToStore.push_back(i);}
}

BDSEventAction::~BDSEventAction()
{;}

void BDSEventAction::BeginOfEventAction(const G4Event* evt)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "processing begin of event action" << G4endl;
#endif
  BDSStackingAction::energyKilled = 0;
  primaryAbsorbedInCollimator = false; // reset flag
  currentEventIndex = evt->GetEventID();
  
  // reset navigators to ensure no mis-navigating and that events are truly independent
  BDSAuxiliaryNavigator::ResetNavigatorStates();
  // make further attempts to clear Geant4's tracking history between events to make them
  // truly independent.
  G4Navigator* trackingNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  trackingNavigator->ResetStackAndState();
  G4TransportationManager* tm = G4TransportationManager::GetTransportationManager();
  int i = 0;
  for (auto it = tm->GetActiveNavigatorsIterator(); i < (int)tm->GetNoActiveNavigators(); it++)
    {(*it)->ResetStackAndState(); i++;}
  tm->GetPropagatorInField()->ClearPropagatorState(); // <- this one really makes a difference

  // Unfortunately the interfaces to G4TransportationManager aren't great which makes this a bit
  // pedantic. Also, the GetNavigator creates an exception if it doesn't find what it's looking
  // for rather than just return a nullptr
  G4bool samplerWorldExists =  false;
  std::vector<G4VPhysicalVolume*>::iterator worldIterator = tm->GetWorldsIterator();
  for (G4int iw = 0; iw < (G4int)tm->GetNoWorlds(); iw++)
    {
      samplerWorldExists = samplerWorldExists || (*worldIterator)->GetName() == "SamplerWorld_main";
      worldIterator++;
    }
  if (samplerWorldExists)
    {
      auto swtracker = tm->GetNavigator("SamplerWorld_main");
      if (swtracker)
        {swtracker->ResetStackAndState();}
    }
  fpEventManager->GetStackManager()->clear();
  
  // update reference to event info
  eventInfo = static_cast<BDSEventInfo*>(evt->GetUserInformation());

  // number feedback
  G4int event_number = evt->GetEventID();
  BDSSDTerminator::eventNumber = event_number; // update static member of terminator
  eventInfo->SetIndex(event_number);
  if (event_number%printModulo == 0)
    {G4cout << "---> Begin of event: " << event_number << G4endl;}
  if (verboseEventBDSIM) // always print this out
    {G4cout << __METHOD_NAME__ << "event #" << event_number << G4endl;}

  // cache hit collection IDs for quicker access
  if (samplerCollID_plane < 0)
    { // if one is -1 then all need initialised.
      G4SDManager*  g4SDMan  = G4SDManager::GetSDMpointer();
      BDSSDManager* bdsSDMan = BDSSDManager::Instance();
      samplerCollID_plane      = g4SDMan->GetCollectionID(bdsSDMan->SamplerPlane()->GetName());
      samplerCollID_cylin      = g4SDMan->GetCollectionID(bdsSDMan->SamplerCylinder()->GetName());
      eCounterID               = g4SDMan->GetCollectionID(bdsSDMan->EnergyDeposition()->GetName());
      eCounterFullID           = g4SDMan->GetCollectionID(bdsSDMan->EnergyDepositionFull()->GetName());
      eCounterVacuumID         = g4SDMan->GetCollectionID(bdsSDMan->EnergyDepositionVacuum()->GetName());
      eCounterTunnelID         = g4SDMan->GetCollectionID(bdsSDMan->EnergyDepositionTunnel()->GetName());
      eCounterWorldID          = g4SDMan->GetCollectionID(bdsSDMan->EnergyDepositionWorld()->GetName());
      eCounterWorldContentsID  = g4SDMan->GetCollectionID(bdsSDMan->EnergyDepositionWorldContents()->GetName());
      worldExitCollID          = g4SDMan->GetCollectionID(bdsSDMan->WorldExit()->GetName());
      collimatorCollID         = g4SDMan->GetCollectionID(bdsSDMan->Collimator()->GetName());
      apertureCollID           = g4SDMan->GetCollectionID(bdsSDMan->ApertureImpacts()->GetName());
      thinThingCollID          = g4SDMan->GetCollectionID(bdsSDMan->ThinThing()->GetName());
    }
  FireLaserCompton=true;

  cpuStartTime = std::clock();
  // get the current time - last thing before we hand off to geant4
  startTime = time(nullptr);
  eventInfo->SetStartTime(startTime);
  eventInfo->SetStopTime(startTime); // initialise to duration of 0

  milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  starts = (G4double)ms.count()/1000.0;
}

void BDSEventAction::EndOfEventAction(const G4Event* evt)
{
  auto flagsCache(G4cout.flags());
  // Get event number information
  G4int event_number = evt->GetEventID();
  G4bool verboseThisEvent = verboseEventBDSIM && BDS::VerboseThisEvent(event_number, verboseEventStart, verboseEventStop);
#ifdef BDSDEBUG
  verboseThisEvent = true; // force on for debug mode
#endif
  const G4int nChar = 50; // for print out
  if (verboseThisEvent)
    {G4cout << __METHOD_NAME__ << "processing end of event"<<G4endl;}
  eventInfo->SetIndex(event_number);

  // Record if event was aborted - ie whether it's useable for analyses.
  eventInfo->SetAborted(evt->IsAborted());

  // Calculate the elapsed CPU time for the event.
  auto cpuEndTime = std::clock();
  G4float durationCPU = static_cast<G4float>(cpuEndTime - cpuStartTime) / CLOCKS_PER_SEC;
  eventInfo->SetDurationCPU(durationCPU);

  // Get the current wall time
  stopTime = time(nullptr);
  eventInfo->SetStopTime(stopTime);

  // Timing information (wall)
  milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  stops = (G4double)ms.count()/1000.0;
  eventInfo->SetDurationWall(G4float(stops - starts));

  G4double memoryUsedMb = BDS::GetMemoryUsage();
  eventInfo->SetMemoryUsage(memoryUsedMb);

  // cache if primary was absorbed in a collimator
  eventInfo->SetPrimaryAbsorbedInCollimator(primaryAbsorbedInCollimator);

  // feedback
  if (verboseThisEvent)
    {eventInfo->Print();}

  // Get the hits collection of this event - all hits from different SDs.
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();

  // samplers
  typedef BDSHitsCollectionSampler shc;
  shc* SampHC       = HCE ? dynamic_cast<shc*>(HCE->GetHC(samplerCollID_plane)) : nullptr;
  shc* hitsCylinder = HCE ? dynamic_cast<shc*>(HCE->GetHC(samplerCollID_cylin)) : nullptr;

  // energy deposition collections - eloss, tunnel hits
  typedef BDSHitsCollectionEnergyDeposition echc;
  echc* eCounterHits       = HCE ? dynamic_cast<echc*>(HCE->GetHC(eCounterID)) : nullptr;
  echc* eCounterFullHits   = HCE ? dynamic_cast<echc*>(HCE->GetHC(eCounterFullID)) : nullptr;
  echc* eCounterVacuumHits = HCE ? dynamic_cast<echc*>(HCE->GetHC(eCounterVacuumID)) : nullptr;
  echc* eCounterTunnelHits = HCE ? dynamic_cast<echc*>(HCE->GetHC(eCounterTunnelID)) : nullptr;

  // world exit hits
  typedef BDSHitsCollectionEnergyDepositionGlobal ecghc;
  ecghc* eCounterWorldHits          = HCE ? dynamic_cast<ecghc*>(HCE->GetHC(eCounterWorldID)) : nullptr;
  ecghc* eCounterWorldContentsHits  = HCE ? dynamic_cast<ecghc*>(HCE->GetHC(eCounterWorldContentsID)) : nullptr;
  ecghc* worldExitHits              = HCE ? dynamic_cast<ecghc*>(HCE->GetHC(worldExitCollID)) : nullptr;

  // aperture hits
  typedef BDSHitsCollectionApertureImpacts aihc;
  aihc* apertureImpactHits = HCE ? dynamic_cast<aihc*>(HCE->GetHC(apertureCollID)) : nullptr;

  // thin thing hits
  typedef BDSHitsCollectionThinThing tthc;
  tthc* thinThingHits = HCE ? dynamic_cast<tthc*>(HCE->GetHC(thinThingCollID)) : nullptr;
  
  // primary hit something?
  // we infer this by seeing if there are any energy deposition hits at all - if there
  // are, the primary must have 'hit' something. possibly along step ionisation in vacuum
  // may fool this..
  if (eCounterHits)
    {
      if (verboseThisEvent)
	{G4cout << std::left << std::setw(nChar) << "Energy deposition hits: " << eCounterHits->entries() << G4endl;}
      if (eCounterHits->entries() > 0)
	{eventInfo->SetPrimaryHitMachine(true);}
    }
  if (eCounterFullHits)
    {
      if (verboseThisEvent)
	{G4cout << std::left << std::setw(nChar) << "Energy deposition full hits: " << eCounterFullHits->entries() << G4endl;}
      if (eCounterFullHits->entries() > 0)
	{eventInfo->SetPrimaryHitMachine(true);}
    }
  if (eCounterTunnelHits)
    {
      if (verboseThisEvent)
	{G4cout << std::left << std::setw(nChar) << "Tunnel energy deposition hits: " << eCounterTunnelHits->entries() << G4endl;}
      if (eCounterTunnelHits->entries() > 0)
	{eventInfo->SetPrimaryHitMachine(true);}
    }
  // we don't check the world energy hits here because the hits could be from
  // intended transport through air in part of the machine (a gap).
  // similarly, there could be ionisation of the vacuum gas without a real impact
  // so we don't check the vacuum energy deposition

  // collimator hits if any
  typedef BDSHitsCollectionCollimator chc;
  chc* collimatorHits = HCE ? dynamic_cast<chc*>(HCE->GetHC(collimatorCollID)) : nullptr;

  if (verboseThisEvent)
    {
      if (eCounterVacuumHits)
        {G4cout << std::left << std::setw(nChar) << "Vacuum energy deposition hits: " << eCounterVacuumHits->entries() << G4endl;}
      if (eCounterWorldHits)
	{G4cout << std::left << std::setw(nChar) << "World energy deposition hits: " << eCounterWorldHits->entries() << G4endl;}
      if (eCounterWorldContentsHits)
	{G4cout << std::left << std::setw(nChar) << "World contents energy deposition hits: " << eCounterWorldContentsHits->entries() << G4endl;}
      if (worldExitHits)
	{G4cout << std::left << std::setw(nChar) << "World exit hits: " << worldExitHits->entries() << G4endl;}
      if (collimatorHits)
	{G4cout << std::left << std::setw(nChar) << "Collimator hits: " << collimatorHits->entries()  << G4endl;}
    }
  
  // primary hits and losses from
  const BDSTrajectoryPoint* primaryHit  = nullptr;
  const BDSTrajectoryPoint* primaryLoss = nullptr;
  G4TrajectoryContainer* trajCont = evt->GetTrajectoryContainer();
  if (trajCont)
    {
      if (verboseThisEvent)
	{G4cout << std::left << std::setw(nChar) << "Trajectories: " << trajCont->size() << G4endl;}
      BDSTrajectoryPrimary* primary = BDS::GetPrimaryTrajectory(trajCont);
      primaryHit  = primary->FirstHit();
      primaryLoss = primary->LastPoint();
    }

  if (thinThingHits)
    {// thinThingHits might be nullptr
      if (thinThingHits->entries() > 0)
	{// if no thin hits, no more information we can add so continue
	  // get all thin hits (may be multiple), include the existing primary hit, and sort them
	  std::vector<const BDSTrajectoryPoint*> points = BDSHitThinThing::TrajectoryPointsFromHC(thinThingHits);
	  if (primaryHit) // if there is a primary hit, add it to the vector for sorting
	    {points.push_back(primaryHit);}

	  // use a lambda function to compare contents of pointers and not pointers themselves
	  auto TrajPointComp = [](const BDSTrajectoryPoint* a, const BDSTrajectoryPoint* b)
			       {
				 if (!a || !b)
				   {return false;}
				 else
				   {return *a < *b;}
			       };
	  // find the earliest on hit
	  std::sort(points.begin(), points.end(), TrajPointComp);
	  primaryHit = points[0]; // use this as the primary hit
	  // note geant4 cleans up hits
	}
    }

  BDSTrajectoriesToStore* interestingTrajectories = IdentifyTrajectoriesForStorage(evt,
										   verboseEventBDSIM || verboseThisEvent,
										   eCounterHits,
										   eCounterFullHits,
										   SampHC,
										   nChar);

  output->FillEvent(eventInfo,
		    evt->GetPrimaryVertex(),
		    SampHC,
		    hitsCylinder,
		    eCounterHits,
		    eCounterFullHits,
		    eCounterVacuumHits,
		    eCounterTunnelHits,
		    eCounterWorldHits,
		    eCounterWorldContentsHits,
		    worldExitHits,
		    primaryHit,
		    primaryLoss,
		    interestingTrajectories,
		    collimatorHits,
		    apertureImpactHits,
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
	
  if (verboseThisEvent)
    {
      G4cout << __METHOD_NAME__ << "end of event action done" << G4endl;
      G4cout << "Energy deposition pool size:        " << BDSAllocatorEnergyDeposition.GetAllocatedSize()  << G4endl;
      G4cout << "Energy deposition extra pool size:  " << BDSAllocatorEnergyDepositionExtra.GetAllocatedSize() << G4endl;
      G4cout << "Collimator hits pool size:          " << BDSAllocatorCollimator.GetAllocatedSize()        << G4endl;
      G4cout << "Trajectory pool size:               " << bdsTrajectoryAllocator.GetAllocatedSize()        << G4endl;
      G4cout << "Trajectory point pool size bdsim:   " << bdsTrajectoryPointAllocator.GetAllocatedSize()   << G4endl;
#if G4VERSION_NUMBER > 1049
      G4cout << "Trajectory point pool size:         " << aTrajectoryPointAllocator()->GetAllocatedSize()  << G4endl;
#else
      G4cout << "Trajectory point pool size:         " << aTrajectoryPointAllocator->GetAllocatedSize()    << G4endl;
#endif
      G4cout << "Trajectory point primary pool size: " << bdsTrajectoryPrimaryAllocator.GetAllocatedSize() << G4endl;
    }

  delete interestingTrajectories;
  
  G4cout.flags(flagsCache);
}

BDSTrajectoriesToStore* BDSEventAction::IdentifyTrajectoriesForStorage(const G4Event* evt,
								       G4bool verbose,
								       BDSHitsCollectionEnergyDeposition* eCounterHits,
								       BDSHitsCollectionEnergyDeposition* eCounterFullHits,
								       BDSHitsCollectionSampler* SampHC,
								       G4int                     nChar) const
{
  G4TrajectoryContainer* trajCont = evt->GetTrajectoryContainer();
  
  // Save interesting trajectories
  std::map<BDSTrajectory*, bool> interestingTraj;
  std::map<BDSTrajectory*, std::bitset<BDS::NTrajectoryFilters> > trajectoryFilters;

  if (storeTrajectory && trajCont)
    {
      TrajectoryVector* trajVec = trajCont->GetVector();
      
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
      
      // fill parent pointer - this can only be done once the map in the previous loop has been made
      for (auto iT1 : *trajVec) 
	{
	  BDSTrajectory* traj = static_cast<BDSTrajectory*>(iT1);	
	  traj->SetParent(trackIDMap[iT1->GetParentID()]);
	}
      
      // loop over trajectories and determine if it should be stored
      // keep tallies of how many will and won't be stored
      G4int nYes = 0;
      G4int nNo  = 0;
      for (auto iT1 : *trajVec)
	{
	  std::bitset<BDS::NTrajectoryFilters> filters;
	  
	  BDSTrajectory* traj = static_cast<BDSTrajectory*>(iT1);
	  G4int parentID = traj->GetParentID();
	  
	  // always store primaries
	  if (parentID == 0)
	    {filters[BDSTrajectoryFilter::primary] = true;}
	  
	  // check on energy (if energy threshold is not negative)
	  if (trajectoryEnergyThreshold >= 0 &&
	      traj->GetInitialKineticEnergy() > trajectoryEnergyThreshold)
	    {filters[BDSTrajectoryFilter::energyThreshold] = true;}
	  
	  // check on particle if not empty string
	  if (!trajParticleNameToStore.empty() || !trajParticleIDToStore.empty())
	    {
	      G4String particleName  = traj->GetParticleName();
	      G4int particleID       = traj->GetPDGEncoding();
	      G4String particleIDStr = G4String(std::to_string(particleID));
	      std::size_t found1     = trajParticleNameToStore.find(particleName);
	      bool        found2     = (std::find(trajParticleIDIntToStore.begin(), trajParticleIDIntToStore.end(), particleID)
                                    != trajParticleIDIntToStore.end());
	      if ((found1 != std::string::npos) || found2)
		{filters[BDSTrajectoryFilter::particle] = true;}
	    }
	  
	  // check on trajectory tree depth (trajDepth = 0 means only primaries)
	  if (depthMap[traj] <= trajDepth || storeTrajectoryAll) // all means to infinite trajDepth really
	    {filters[BDSTrajectoryFilter::depth] = true;}
	  
	  // check on coordinates (and TODO momentum)
	  // clear out trajectories that don't reach point TrajCutGTZ or greater than TrajCutLTR
	  BDSTrajectoryPoint* trajEndPoint = static_cast<BDSTrajectoryPoint*>(traj->GetPoint(traj->GetPointEntries() - 1));

	  // end point greater than some Z
	  if (trajEndPoint->GetPosition().z() > trajectoryCutZ)
	    {filters[BDSTrajectoryFilter::minimumZ] = true;}
	  
	  // less than maximum R
	  if (trajEndPoint->PostPosR() < trajectoryCutR)
	    {filters[BDSTrajectoryFilter::maximumR] = true;}
	  
	  filters.any() ? nYes++ : nNo++;
	  interestingTraj.insert(std::pair<BDSTrajectory*, bool>(traj, filters.any()));
	  trajectoryFilters.insert(std::pair<BDSTrajectory*, std::bitset<BDS::NTrajectoryFilters> >(traj, filters)); 
	}
      
      // loop over energy hits to connect trajectories
      if (trajSRangeToStore.size() != 0)
	{
	  if (eCounterHits)
	    {
	      G4int nHits = eCounterHits->entries();
	      BDSHitEnergyDeposition* hit;
	      for (G4int i = 0; i < nHits; i++)
		{
		  hit = (*eCounterHits)[i];
		  double dS = hit->GetSHit();
		  for (const auto& v : trajSRangeToStore)
		    {		
		      if ( dS >= v.first && dS <= v.second) 
			{
			  BDSTrajectory* trajToStore = trackIDMap[hit->GetTrackID()];
			  if (!interestingTraj[trajToStore])
			    {// was marked as not storing - update counters
			      nYes++;
			      nNo--;
			    }
			  interestingTraj[trajToStore] = true;
			  trajectoryFilters[trajToStore][BDSTrajectoryFilter::elossSRange] = true;
			  break;
			}
		    }
		}
	    }
	  if (eCounterFullHits)
	    {
	      G4int nHits = eCounterFullHits->entries();
	      BDSHitEnergyDeposition* hit;
	      for (G4int i = 0; i < nHits; i++)
		{
		  hit = (*eCounterFullHits)[i];
		  double dS = hit->GetSHit();
		  for (const auto& v : trajSRangeToStore)
		    {		
		      if ( dS >= v.first && dS <= v.second) 
			{
			  BDSTrajectory* trajToStore = trackIDMap[hit->GetTrackID()];
			  if (!interestingTraj[trajToStore])
			    {// was marked as not storing - update counters
			      nYes++;
			      nNo--;
			    }
			  interestingTraj[trajToStore] = true;
			  trajectoryFilters[trajToStore][BDSTrajectoryFilter::elossSRange] = true;
			  break;
			}
		    }
		}
	    }
	}
      
      // loop over samplers to connect trajectories
      if (!trajectorySamplerID.empty() && SampHC)
	{
	  G4int nHits = SampHC->entries();
	  for (G4int i = 0; i < nHits; i++)
	    {
	      G4int samplerIndex = (*SampHC)[i]->samplerID;
	      BDSSamplerInfo info = BDSSamplerRegistry::Instance()->GetInfo(samplerIndex);
	      if (std::find(trajectorySamplerID.begin(), trajectorySamplerID.end(), samplerIndex) != trajectorySamplerID.end())
		{
		  BDSTrajectory* trajToStore = trackIDMap[(*SampHC)[i]->trackID];
		  if (!interestingTraj[trajToStore])
		    {// was marked as not storing - update counters
		      nYes++;
		      nNo--;
		    }
		  interestingTraj[trajToStore] = true;
		  trajectoryFilters[trajToStore][BDSTrajectoryFilter::sampler] = true;
		}
	    }
	}

	// If we're using AND logic (default OR) with the filters, loop over and update whether
	// we should really store the trajectory or not. Importantly, we do this before the connect
	// trajectory step as that flags yet more trajectories (that connect each one) back to the
	// primary
	if (trajectoryFilterLogicAND)
      {
        for (auto& trajFlag : interestingTraj)
          {
            if (trajFlag.second) // if we're going to store it check the logic
              {
                // use bit-wise AND filters matched for this trajectory with filters set
                // if count of 1s the same, then trajectory should be stored, therefore if
                // not the same, it should be set to false.
                auto filterMatch = trajectoryFilters[trajFlag.first] & trajFiltersSet;
                if (filterMatch.count() != trajFiltersSet.count())
                  {trajFlag.second = false;}
              }
          }

      }
      
      // Connect trajectory graphs
      if (trajConnect && trackIDMap.size() > 1)
	{
	  for (auto i : interestingTraj)
	    if (i.second) 
	      {connectTraj(interestingTraj, i.first);}
	}
      // Output interesting trajectories
      if (verbose)
	{G4cout << std::left << std::setw(nChar) << "Trajectories for storage: " << nYes << " out of " << nYes + nNo << G4endl;}
    }
  
  return new BDSTrajectoriesToStore(interestingTraj, trajectoryFilters);
}
