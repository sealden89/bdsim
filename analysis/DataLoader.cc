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
#include "DataLoader.hh"
#include "Beam.hh"
#include "Event.hh"
#include "FileMapper.hh"
#include "Geant4Data.hh"
#include "Header.hh"
#include "Model.hh"
#include "Options.hh"
#include "RebdsimTypes.hh"
#include "Run.hh"

#include "BDSDebug.hh"
#include "BDSOutputROOTEventCollimator.hh"
#include "BDSOutputROOTEventSampler.hh"

#include "TChain.h"
#include "TFile.h"

#include <algorithm>
#include <cmath>
#include <glob.h>
#include <iostream>

ClassImp(DataLoader)

DataLoader::DataLoader(std::string fileName,
		       bool        debugIn,
		       bool        processSamplersIn,
		       bool        allBranchesOnIn,
		       const RBDS::BranchMap* branchesToTurnOnIn,
		       bool        backwardsCompatibleIn):
  debug(debugIn),
  processSamplers(processSamplersIn),
  allBranchesOn(allBranchesOnIn),
  branchesToTurnOn(branchesToTurnOnIn),
  backwardsCompatible(backwardsCompatibleIn),
  dataVersion(4)
{
  CommonCtor(fileName);
}

DataLoader::~DataLoader()
{
  delete hea;
  delete bea;
  delete opt;
  delete mod;
  delete evt;
  delete run;

  delete heaChain;
  delete beaChain;
  delete optChain;
  delete modChain;
  delete evtChain;
  delete runChain;
}

void DataLoader::CommonCtor(std::string fileName)
{
  BuildInputFileList(fileName);

  hea = new Header(debug);
  g4d = new Geant4Data(debug);
  bea = new Beam(debug);
  opt = new Options(debug);
  mod = new Model(debug, dataVersion);
  evt = new Event(debug, processSamplers, dataVersion);
  run = new Run(debug, dataVersion);
  
  heaChain = new TChain("Header",      "Header");
  if (!backwardsCompatible)
    {g4dChain = new TChain("Geant4Data", "Geant4Data");}
  beaChain = new TChain("Beam",       "Beam");
  optChain = new TChain("Options",    "Options");
  modChain = new TChain("Model",      "Model");
  evtChain = new TChain("Event",      "Event");
  runChain = new TChain("Run",        "Run");

  BuildTreeNameList();
  BuildEventBranchNameList();
  ChainTrees();
  SetBranchAddress(allBranchesOn, branchesToTurnOn);

  if (!backwardsCompatible)
    {
      g4dChain->GetEntry(0); // load particle data
#ifdef __ROOTDOUBLE__
      BDSOutputROOTEventSampler<double>::particleTable = g4d->geant4Data;
#else
      BDSOutputROOTEventSampler<float>::particleTable = g4d->geant4Data;
      BDSOutputROOTEventCollimator::particleTable = g4d->geant4Data;
#endif
    }
}

void DataLoader::BuildInputFileList(std::string inputPath)
{
  if(inputPath.empty())
    {throw std::string("DataLoader::BuildInputFileList> no file specified");}

  // wild card
  std::vector<std::string> fileNamesTemp;
  if(inputPath.find("*") != std::string::npos)
    {
      glob_t glob_result;
      glob(inputPath.c_str(),GLOB_TILDE,nullptr,&glob_result);
      for(unsigned int i=0;i<glob_result.gl_pathc;++i)
	{fileNamesTemp.push_back(glob_result.gl_pathv[i]);}
      globfree(&glob_result);
    }
  // single file
  else if(inputPath.find(".root") != std::string::npos)
    {fileNamesTemp.push_back(inputPath);}
  // directory
  else if(inputPath[inputPath.length()-1] == std::string("/"))
    {
      // find all files in directory
      inputPath.append("/*.root");
      
      glob_t glob_result;
      glob(inputPath.c_str(),GLOB_TILDE,nullptr,&glob_result);
      for(unsigned int i=0;i<glob_result.gl_pathc;++i)
	{fileNamesTemp.push_back(glob_result.gl_pathv[i]);}
      globfree(&glob_result);
    }

  // loop over files and check they're the right type
  int* fileDataVersion = nullptr; // for backwards compatibility don't try
  if (!backwardsCompatible)
    {
      fileDataVersion = new int();
      (*fileDataVersion) = 0;
    }
  for (const auto& fn : fileNamesTemp)
    {
      if (backwardsCompatible)
	{fileNames.push_back(fn);} // don't check if header -> old files don't have this
      else if (RBDS::IsBDSIMOutputFile(fn, fileDataVersion))
	{
	  std::cout << "Loading> \"" << fn << "\" : data version " << *fileDataVersion << std::endl;
	  fileNames.push_back(fn);
	  dataVersion = std::min(dataVersion, *fileDataVersion);
	}
      else
	{std::cout << fn << " is not a BDSIM output file - skipping!" << std::endl;}
    }
  delete fileDataVersion;
  
  if (fileNames.empty()) // exit if no valid files.
    {
      std::cout << "DataLoader - No valid files found - check input file path / name" << std::endl;
      exit(1);
    }
}

void DataLoader::BuildTreeNameList()
{
  // open file - this is the first opening so test here if it's valid
  TFile* f = new TFile(fileNames[0].c_str());
  if (f->IsZombie())
    {
      std::cout << __METHOD_NAME__ << " no such file \"" << fileNames[0] << "\"" << std::endl;
      exit(1);
    }
  
  TList* kl = f->GetListOfKeys();
  for (int i = 0; i < kl->GetEntries(); ++i)
    {treeNames.push_back(std::string(kl->At(i)->GetName()));}

  f->Close();
  delete f;

  if(debug)
    {
      for (const auto& tr : treeNames)
	{std::cout << "DataLoader::BuildTreeNameList> " <<  tr << std::endl;}
    }
}

void DataLoader::BuildEventBranchNameList()
{
  TFile* f = new TFile(fileNames[0].c_str());
  if (f->IsZombie())
    {
      std::cout << __METHOD_NAME__ << " no such file \"" << fileNames[0] << "\"" << std::endl;
      exit(1);
    }

  // We don't need to prepare a vector of samplers that will be set branch on
  // if we're not going to process the samplers.
  if (!processSamplers)
    {return;}
  
  TTree* mt = (TTree*)f->Get("Model");
  if (!mt)
    {return;}

  Model* modTemporary = new Model();
  modTemporary->SetBranchAddress(mt);
  mt->GetEntry(0);
  samplerNames    = modTemporary->SamplerNames(); // copy sampler names out
  // collimator names was only added in data version 4 - can leave as empty vector
  if (dataVersion > 3)
    {collimatorNames = modTemporary->CollimatorNames();}
  
  f->Close();
  delete f;
  delete modTemporary;

  if(debug)
    {
      for (const auto& n : branchNames)
	{std::cout << "DataLoader::BuildEventBranchNameList> Non-sampler : " << n << std::endl;}
      for (const auto& n : samplerNames)
	{std::cout << "DataLoader::BuildEventBranchNameList> Sampler     : " << n << std::endl;}
      for (const auto& n : collimatorNames)
	{std::cout << "DataLoader::BuildEventBranchNameList> Collimator  : " << n << std::endl;}
    }
}

void DataLoader::ChainTrees()
{
  // loop over files and chain trees
  if (!backwardsCompatible)
    {g4dChain->Add(fileNames[0].c_str());} // only require 1 copy
  for (const auto& filename : fileNames)
    {
      heaChain->Add(filename.c_str());
      beaChain->Add(filename.c_str());
      optChain->Add(filename.c_str());
      modChain->Add(filename.c_str());
      evtChain->Add(filename.c_str());
      runChain->Add(filename.c_str());
    }
}

void DataLoader::SetBranchAddress(bool allOn,
				  const RBDS::BranchMap* bToTurnOn)
{
  if (!backwardsCompatible)
    {g4d->SetBranchAddress(g4dChain);}
  hea->SetBranchAddress(heaChain);
  bea->SetBranchAddress(beaChain, true); // true = always turn on all branches
  mod->SetBranchAddress(modChain, true); // true = always turn on all branches
  opt->SetBranchAddress(optChain, true); // true = always turn on all branches
  // note we can't parse the :: properly in the options tree so we turn on by default

  const RBDS::VectorString* evtBranches = nullptr;
  if (bToTurnOn)
    {
      if (bToTurnOn->find("Event.") != bToTurnOn->end())
	{evtBranches = &(*bToTurnOn).at("Event.");}
    }
  evt->SetBranchAddress(evtChain, &samplerNames, allOn, evtBranches, &collimatorNames);

  const RBDS::VectorString* runBranches = nullptr;
  if (bToTurnOn)
    {
      if (bToTurnOn->find("Run.") != bToTurnOn->end())
	{runBranches = &(*bToTurnOn).at("Run.");}
    }
  run->SetBranchAddress(runChain, allOn, runBranches);
}
