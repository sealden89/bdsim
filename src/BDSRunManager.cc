/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSRunManager.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSExtent.hh"
#include "BDSPrimaryGeneratorAction.hh"

#include "CLHEP/Random/Random.h"

BDSRunManager::BDSRunManager()
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "constructing run manager"<<G4endl;
#endif
}

BDSRunManager::~BDSRunManager()
{
#ifdef BDSDEBUG 
  G4cout<< __FUNCTION__ << "> BDSRunManager deleting..."<<G4endl;
#endif
}

void BDSRunManager::Initialize()
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "> Initialising Geant4 kernel"<<G4endl;
#endif
  G4RunManager::Initialize();

  BDSExtent worldExtent;
  if (const auto detectorConstruction = dynamic_cast<BDSDetectorConstruction*>(userDetector))
    {worldExtent = detectorConstruction->WorldExtent();}
  if (const auto primaryGeneratorAction = dynamic_cast<BDSPrimaryGeneratorAction*>(userPrimaryGeneratorAction))
    {primaryGeneratorAction->SetWorldExtent(worldExtent);}
}

void BDSRunManager::BeamOn(G4int n_event,const char* macroFile,G4int n_select)
{
  G4RunManager::BeamOn(n_event,macroFile,n_select);
}

void BDSRunManager::DoEventLoop(G4int n_event,const char* macroFile,G4int n_select)
{
  // save event loop state
  if(verboseLevel>0){
    // Print seed to try and recreate an event in a run 
    G4cout << __METHOD_NAME__ << "Random number generator's seed=" 
	   << CLHEP::HepRandom::getTheSeed() << G4endl;
    // Print generator full state to output 
    G4cout << __METHOD_NAME__ << "Random number generator's state: " << G4endl;
    CLHEP::HepRandom::saveFullState(G4cout);
  }

  G4RunManager::DoEventLoop(n_event,macroFile,n_select);
}

void BDSRunManager::ProcessOneEvent(G4int i_event)
{
  // additional output
  if(verboseLevel>3){
    G4cout << __METHOD_NAME__ << "Event="<<i_event<<G4endl;
    // Print seed to try and recreate an event in a run
    G4cout << __METHOD_NAME__ << "Random number generator's seed=" 
	   << CLHEP::HepRandom::getTheSeed() << G4endl;
    // Print generator full state to output 
    G4cout << __METHOD_NAME__ << "Random number generator's state: " << G4endl;
    CLHEP::HepRandom::saveFullState(G4cout);
  }

  G4RunManager::ProcessOneEvent(i_event);
}

void BDSRunManager::AbortRun(G4bool)
{
  G4cout << "Terminate run - trying to write and close output file" << G4endl;
  G4RunManager::AbortRun();
}

