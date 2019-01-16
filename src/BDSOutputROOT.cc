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
#include "BDSOutputROOT.hh"

#include "parser/options.h"
#include "BDSDebug.hh"
#include "BDSOutputROOTEventBeam.hh"
#include "BDSOutputROOTEventCoords.hh"
#include "BDSOutputROOTEventExit.hh"
#include "BDSOutputROOTEventHeader.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTGeant4Data.hh"

#include "TFile.h"
#include "TObject.h"
#include "TTree.h"

BDSOutputROOT::BDSOutputROOT(G4String fileName,
			     G4int    fileNumberOffset):
  BDSOutput(fileName, ".root", fileNumberOffset),
  theRootOutputFile(nullptr),
  theHeaderOutputTree(nullptr),
  theGeant4DataTree(nullptr),
  theBeamOutputTree(nullptr),
  theOptionsOutputTree(nullptr),
  theModelOutputTree(nullptr),
  theEventOutputTree(nullptr),
  theRunOutputTree(nullptr)
{;}

BDSOutputROOT::~BDSOutputROOT()
{
  if (theRootOutputFile && theRootOutputFile->IsOpen())
    {theRootOutputFile->Write(0,TObject::kOverwrite);}
}

void BDSOutputROOT::NewFile() 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  G4String newFileName = GetNextFileName();
  
  theRootOutputFile = new TFile(newFileName,"RECREATE", "BDS output file");

  if (theRootOutputFile->IsZombie())
    {
      G4cerr << __METHOD_NAME__ << "Unable to open output file: \"" << newFileName << "\"" << G4endl;
      exit(1);
    }
  
  // root file - note this sets the current 'directory' to this file!
  theRootOutputFile->cd();
  
  theHeaderOutputTree  = new TTree("Header", "BDSIM Header");           // header
  theGeant4DataTree    = new TTree("Geant4Data", "BDSIM Geant4 Data");  // geant4 data
  theBeamOutputTree    = new TTree("Beam", "BDSIM beam");               // beam data tree
  theOptionsOutputTree = new TTree("Options","BDSIM options");          // options data tree
  theModelOutputTree   = new TTree("Model","BDSIM model");              // model data tree
  theRunOutputTree     = new TTree("Run","BDSIM run histograms/information"); // run info tree
  theEventOutputTree   = new TTree("Event","BDSIM event");              // event data tree

  // Build branches for each object
  theHeaderOutputTree->Branch("Header.",   "BDSOutputROOTEventHeader",    headerOutput,     32000, 1);
  theGeant4DataTree->Branch("Geant4Data.", "BDSOutputROOTGeant4Data",     geant4DataOutput, 32000, 1);
  theBeamOutputTree->Branch("Beam.",       "BDSOutputROOTEventBeam",      beamOutput,       32000, 2);
  theOptionsOutputTree->Branch("Options.", "BDSOutputROOTEventOptions",   optionsOutput,    32000, 2);
  theModelOutputTree->Branch("Model.",     "BDSOutputROOTEventModel",     modelOutput,      32000, 1);
  theRunOutputTree->Branch("Histos.",      "BDSOutputROOTEventHistograms",runHistos,        32000, 1);
  theRunOutputTree->Branch("Info.",        "BDSOutputROOTEventRunInfo",   runInfo,          32000, 1);

  // Branches for event...
  // Event info output
  theEventOutputTree->Branch("Info.",           "BDSOutputROOTEventInfo",evtInfo,32000,1);

  // Build primary structures
  if (WritePrimaries())
    {
      theEventOutputTree->Branch("Primary.",       "BDSOutputROOTEventSampler",primary,32000,1);
      theEventOutputTree->Branch("PrimaryGlobal.", "BDSOutputROOTEventCoords", primaryGlobal, 3200,1);
    }

  // Build loss and hit structures
  theEventOutputTree->Branch("Eloss.",          "BDSOutputROOTEventLoss", eLoss,      4000, 1);
  theEventOutputTree->Branch("ElossWorld.",     "BDSOutputROOTEventLoss", eLossWorld, 4000, 1);
  theEventOutputTree->Branch("ElossWorldExit.", "BDSOutputROOTEventExit", eLossWorldExit, 4000, 1);
  theEventOutputTree->Branch("PrimaryFirstHit.","BDSOutputROOTEventLoss", pFirstHit,  4000, 2);
  theEventOutputTree->Branch("PrimaryLastHit.", "BDSOutputROOTEventLoss", pLastHit,   4000, 2);
  theEventOutputTree->Branch("TunnelHit.",      "BDSOutputROOTEventLoss", tunnelHit,  4000, 2);

  // Build trajectory structures
  theEventOutputTree->Branch("Trajectory.",     "BDSOutputROOTEventTrajectory",traj,4000,2);

  // Build event histograms
  theEventOutputTree->Branch("Histos.",         "BDSOutputROOTEventHistograms",evtHistos,32000,1);

  // build sampler structures
  for (G4int i = 0; i < (G4int)samplerTrees.size(); ++i)
    {
      auto samplerTreeLocal = samplerTrees.at(i);
      auto samplerName      = samplerNames.at(i);
      // set tree branches
      theEventOutputTree->Branch((samplerName+".").c_str(),
                                 "BDSOutputROOTEventSampler",
                                 samplerTreeLocal,32000,0);
    }

  FillHeader(); // this fills and then calls WriteHeader() pure virtual implemented here
}

void BDSOutputROOT::WriteHeader()
{
  theHeaderOutputTree->Fill();
}

void BDSOutputROOT::WriteGeant4Data()
{
  theGeant4DataTree->Fill();
}

void BDSOutputROOT::WriteBeam()
{
  theBeamOutputTree->Fill();
}

void BDSOutputROOT::WriteOptions()
{
  theOptionsOutputTree->Fill();
}

void BDSOutputROOT::WriteModel()
{
  theModelOutputTree->Fill();
}

void BDSOutputROOT::WriteFileEventLevel()
{
  theRootOutputFile->cd();
  theEventOutputTree->Fill();
}

void BDSOutputROOT::WriteFileRunLevel()
{
  theRootOutputFile->cd();
  theRunOutputTree->Fill();

  if(theRootOutputFile && theRootOutputFile->IsOpen())
    {theRootOutputFile->Write(nullptr,TObject::kOverwrite);}
}

void BDSOutputROOT::CloseFile()
{
  if(theRootOutputFile && theRootOutputFile->IsOpen())
  {
    theRootOutputFile->cd();
    theRootOutputFile->Write(0,TObject::kOverwrite);
    theRootOutputFile->Close();
    delete theRootOutputFile;
    theRootOutputFile = nullptr;
  }
}
