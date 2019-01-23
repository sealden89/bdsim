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
#include "BDSCollimatorSD.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMultiSensitiveDetectorOrdered.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDFilterIon.hh"
#include "BDSSDFilterOr.hh"
#include "BDSSDFilterPrimary.hh"
#include "BDSSDManager.hh"
#include "BDSSDType.hh"
#include "BDSTerminatorSD.hh"
#include "BDSVolumeExitSD.hh"

#include "G4SDManager.hh"
#include "G4Version.hh"

#if G4VERSION_NUMBER > 1029
#include "G4MultiSensitiveDetector.hh"
#endif

BDSSDManager* BDSSDManager::instance = nullptr;

BDSSDManager* BDSSDManager::Instance()
{
  if (!instance)
    {instance = new BDSSDManager();}
  return instance;
}

BDSSDManager::~BDSSDManager()
{
  // no need to delete SD's as they are all registered in G4SDManager
  instance = nullptr;

  for (auto kv : filters)
    {delete kv.second;}
}

BDSSDManager::BDSSDManager()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Constructor - creating all necessary Sensitive Detectors" << G4endl;
#endif
  BDSGlobalConstants* g   = BDSGlobalConstants::Instance();
  stopSecondaries         = g->StopSecondaries();
  verbose                 = g->Verbose();
  storeCollimatorHitsAll  = g->StoreCollimatorHitsAll();
  storeCollimatorHitsIons = g->StoreCollimatorHitsIons();
  generateELossHits       = g->StoreELoss() || g->StoreELossHistograms();
  generateELossVacuumHits = g->StoreELossVacuum() || g->StoreELossVacuumHistograms();
  generateELossTunnelHits = g->StoreELossTunnel() || g->StoreELossTunnelHistograms(); 
  storeELossWorld         = g->StoreELossWorld();
  
  filters["primary"] = new BDSSDFilterPrimary("primary");
  filters["ion"]     = new BDSSDFilterIon("ion");
  BDSSDFilterOr* primaryOrIon = new BDSSDFilterOr("primary_or_ion");
  primaryOrIon->RegisterFilter(filters["primary"]);
  primaryOrIon->RegisterFilter(filters["ion"]);
  filters["primaryorion"] = primaryOrIon;
  
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  
  // sampler plane
  samplerPlane = new BDSSamplerSD("plane");
  SDMan->AddNewDetector(samplerPlane);

  // Sampler cylindrical
  samplerCylinder = new BDSSamplerSD("cylinder");
  SDMan->AddNewDetector(samplerCylinder);

  // Terminator sd to measure how many times that primary has passed through the terminator
  terminator  = new BDSTerminatorSD("terminator");
  SDMan->AddNewDetector(terminator);

  eCounter = new BDSEnergyCounterSD("general", stopSecondaries);
  SDMan->AddNewDetector(eCounter);

  eCounterVacuum = new BDSEnergyCounterSD("vacuum", stopSecondaries);
  SDMan->AddNewDetector(eCounterVacuum);

  eCounterTunnel = new BDSEnergyCounterSD("tunnel", stopSecondaries);
  SDMan->AddNewDetector(eCounterTunnel);

  eCounterWorld = new BDSEnergyCounterSD("worldLoss", stopSecondaries);
  SDMan->AddNewDetector(eCounterWorld);

  worldExit= new BDSVolumeExitSD("worldExit", true);
  SDMan->AddNewDetector(worldExit);

#if G4VERSION_NUMBER > 1029
  // only multiple SDs since 10.3
  G4MultiSensitiveDetector* wcsd = new G4MultiSensitiveDetector("world_complete");
  SDMan->AddNewDetector(wcsd);
  wcsd->AddSD(eCounterWorld);
  wcsd->AddSD(worldExit);
  worldCompleteSD = wcsd;
#endif

  collimatorSD = new BDSCollimatorSD("collimator");
  collimatorCompleteSD = new BDSMultiSensitiveDetectorOrdered("collimator_complete");
  collimatorCompleteSD->AddSD(eCounter);
  collimatorCompleteSD->AddSD(collimatorSD);
  // set up a filter for the collimator sensitive detector - always store primary hits
  G4VSDFilter* filter = nullptr;
  if (storeCollimatorHitsAll)
    {filter = nullptr;} // no filter -> store all
  else if (storeCollimatorHitsIons) // primaries plus ion fragments
    {filter = filters["primaryorion"];}
  else
    {filter = filters["primary"];} // just primaries

  // we only want to attach the filter to the collimator SD and not the energy counter SD
  // via the 'complete' SD. i.e. we always want energy deposition but collimator hits by
  // the filter.
  collimatorSD->SetFilter(filter);
  SDMan->AddNewDetector(collimatorSD);
  SDMan->AddNewDetector(collimatorCompleteSD);
}

G4VSensitiveDetector* BDSSDManager::SensitiveDetector(const BDSSDType sdType,
						      G4bool applyOptions) const
{
  G4VSensitiveDetector* result = nullptr;
  switch (sdType.underlying())
    {
    case BDSSDType::samplerplane:
      {result = samplerPlane; break;}
    case BDSSDType::samplercylinder:
      {result = samplerCylinder; break;}
    case BDSSDType::terminator:
      {result = terminator; break;}
    case BDSSDType::energydep:
      {
	if (applyOptions)
	  {result = generateELossHits ? eCounter : nullptr;}
	else
	  {result = eCounter;}
	break;
      }
    case BDSSDType::energydepvacuum:
      {
	if (applyOptions)
	  {result = generateELossVacuumHits ? eCounterVacuum : nullptr;}
	else
	  {result = eCounterVacuum;}
	break;
      }
    case BDSSDType::energydeptunnel:
      {
	if (applyOptions)
	  {result = generateELossTunnelHits ? eCounterTunnel : nullptr;}
	else
	  {result = eCounterTunnel;}
	break;
      }
    case BDSSDType::energydepworld:
      {
	if (applyOptions)
	  {result = storeELossWorld ? eCounterWorld : nullptr;}
	else
	  {result = eCounterWorld;}
	break;
      }
    case BDSSDType::worldexit:
      {result = worldExit; break;}
    case BDSSDType::worldcomplete:
#if G4VERSION_NUMBER > 1029
      {result = worldCompleteSD; break;}
#else
      {result = nullptr; break;}
#endif
    case BDSSDType::collimator:
      {result = collimatorSD; break;}
    case BDSSDType::collimatorcomplete:
      {result = collimatorCompleteSD; break;}
    default:
      {result = nullptr; break;}
    }
  return result;
}
