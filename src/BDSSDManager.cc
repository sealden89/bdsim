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
#include "BDSDebug.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDManager.hh"
#include "BDSTerminatorSD.hh"
#include "BDSVolumeExitSD.hh"

#include "G4SDManager.hh"
#include "G4Version.hh"

#if G4VERSION_NUMBER > 1029
#include "G4MultiSensitiveDetector.hh"
#endif

BDSSDManager* BDSSDManager::_instance = nullptr;

BDSSDManager* BDSSDManager::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSSDManager();}
  return _instance;
}

BDSSDManager::~BDSSDManager()
{
  // no need to delete SD's as they are all registered in G4SDManager
  _instance = nullptr;
}

BDSSDManager::BDSSDManager()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Constructor - creating all necessary Sensitive Detectors" << G4endl;
#endif
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

  G4bool stopSecondaries = BDSGlobalConstants::Instance()->StopSecondaries();

  eCounter = new BDSEnergyCounterSD("general", stopSecondaries);
  SDMan->AddNewDetector(eCounter);

  tunnelECounter = new BDSEnergyCounterSD("tunnel", stopSecondaries);
  SDMan->AddNewDetector(tunnelECounter);

  worldECounter = new BDSEnergyCounterSD("worldLoss", stopSecondaries);
  SDMan->AddNewDetector(worldECounter);

  worldExit= new BDSVolumeExitSD("worldExit", true);
  SDMan->AddNewDetector(worldExit);

#if G4VERSION_NUMBER > 1029
  // only multiple SDs since
  G4MultiSensitiveDetector* wcsd = new G4MultiSensitiveDetector("world_complete");
  SDMan->AddNewDetector(wcsd);
  wcsd->AddSD(worldECounter);
  wcsd->AddSD(worldExit);
  worldCompleteSD = wcsd;
#endif
}
