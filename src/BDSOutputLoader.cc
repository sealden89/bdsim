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
#include "BDSOutputLoader.hh"
#include "BDSOutputROOTEventBeam.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventOptions.hh"

#include "parser/beam.h"
#include "parser/beamBase.h"
#include "parser/options.h"
#include "parser/optionsBase.h"

#include "globals.hh" // geant4 types / globals

#include "TFile.h"
#include "TList.h"
#include "TTree.h"


BDSOutputLoader::BDSOutputLoader(G4String filePath):
  badFilePath(true),
  rootEventFile(false),
  localBeam(nullptr),
  localOptions(nullptr),
  localEventInfo(nullptr),
  beamTree(nullptr),
  optionsTree(nullptr),
  eventTree(nullptr)
{
  // open file - READ mode to prevent accidental corruption by adding new things
  file = new TFile(filePath.c_str(), "READ");
  
  // check it's a valid file
  badFilePath = file->IsZombie();
  if (badFilePath)
    {
      G4cout << __METHOD_NAME__ << "No such file \"" << filePath << "\"" << G4endl;
      exit(1);
    }
  else
    {// check it's a rootevent file
      rootEventFile = file->GetListOfKeys()->Contains("Event");
      if (!rootEventFile)
	{
	  G4cout << __METHOD_NAME__ << "Not a BDSIM rootevent output format ROOT file" << G4endl;
	  exit(1);
	}
    }

  beamTree = static_cast<TTree*>(file->Get("Beam"));
  if (!beamTree)
    {G4cerr << "Invalid file \"" << filePath << "\" - doesn't contain beam Tree" << G4endl; exit(1);}
  localBeam = new BDSOutputROOTEventBeam();
  beamTree->SetBranchAddress("Beam.", &localBeam);

  // set up local structure copies.
  optionsTree = static_cast<TTree*>(file->Get("Options"));
  if (!optionsTree)
    {G4cerr << "Invalid file \"" << filePath << "\" - doesn't contain options structure" << G4endl; exit(1);}
  localOptions = new BDSOutputROOTEventOptions();
  optionsTree->SetBranchAddress("Options.", &localOptions);
  
  eventTree = static_cast<TTree*>(file->Get("Event"));
  localEventInfo = new BDSOutputROOTEventInfo();
  eventTree->SetBranchAddress("Info.", &localEventInfo);
}

BDSOutputLoader::~BDSOutputLoader()
{
  delete file; // closes if open
  delete localBeam;
  delete localOptions;
  delete localEventInfo;
}

GMAD::BeamBase BDSOutputLoader::BeamBaseClass()
{
  // always change back to this file - assuming other root files could be open
  file->cd();
  optionsTree->GetEntry(0);
  return *(static_cast<GMAD::BeamBase*>(localBeam));
}

GMAD::Beam BDSOutputLoader::Beam()
{
  return GMAD::Beam(BeamBaseClass());
}

GMAD::OptionsBase BDSOutputLoader::OptionsBaseClass()
{
  // always change back to this file - assuming other root files could be open
  file->cd();
  optionsTree->GetEntry(0);
  return *(static_cast<GMAD::OptionsBase*>(localOptions));
}

GMAD::Options BDSOutputLoader::Options()
{
  return GMAD::Options(OptionsBaseClass());
}

G4String BDSOutputLoader::SeedState(G4int eventNumber)
{
  // always change back to this file - assuming other root files could be open
  file->cd();

  // cannot retrieve a seed state beyond that in the file - protection here to
  // make life simpler elsewhere
  if (eventNumber > eventTree->GetEntries())
    {
      G4cout << __METHOD_NAME__ << "event index beyond number stored in file - no seed state loaded" << G4endl;
      return "";
    }
  eventTree->GetEntry((int)eventNumber);
  
  return G4String(localEventInfo->seedStateAtStart);
}
