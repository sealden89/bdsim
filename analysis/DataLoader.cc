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
#include "DataLoader.hh"
#include "Beam.hh"
#include "Event.hh"
#include "FileMapper.hh"
#include "ParticleData.hh"
#include "Header.hh"
#include "Model.hh"
#include "Options.hh"
#include "RBDSException.hh"
#include "RebdsimTypes.hh"
#include "Run.hh"

#include "BDSDebug.hh"
#include "BDSOutputROOTEventAperture.hh"
#include "BDSOutputROOTEventCollimator.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSVersionData.hh"

#include "TChain.h"
#include "TFile.h"

#include <algorithm>
#include <cmath>
#include <glob.h>
#include <iostream>

ClassImp(DataLoader)

DataLoader::DataLoader(const std::string& fileName,
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
  parChain(nullptr),
  dataVersion(BDSIM_DATA_VERSION)
{
  CommonCtor(fileName);
}

DataLoader::~DataLoader()
{
  delete hea;
  delete par;
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

void DataLoader::CommonCtor(const std::string& fileName)
{
  BuildInputFileList(fileName); // updates dataVersion

  hea = new Header(debug);
  par = new ParticleData(debug);
  bea = new Beam(debug);
  opt = new Options(debug);
  mod = new Model(debug, dataVersion);
  evt = new Event(debug, processSamplers, dataVersion);
  run = new Run(debug, dataVersion);
  
  heaChain = new TChain("Header",      "Header");
  if (dataVersion > 6)
    {parChain = new TChain("ParticleData", "ParticleData");}
  beaChain = new TChain("Beam",       "Beam");
  optChain = new TChain("Options",    "Options");
  modChain = new TChain("Model",      "Model");
  evtChain = new TChain("Event",      "Event");
  runChain = new TChain("Run",        "Run");

  BuildTreeNameList();
  BuildEventBranchNameList();
  ChainTrees();
  SetBranchAddress(allBranchesOn, branchesToTurnOn);

  if (dataVersion > 6)
    {
      parChain->GetEntry(0); // load particle data
#ifdef __ROOTDOUBLE__
      BDSOutputROOTEventSampler<double>::particleTable = par->particleData;
#else
      BDSOutputROOTEventSampler<float>::particleTable = par->particleData;
      BDSOutputROOTEventCollimator::particleTable = par->particleData;
      BDSOutputROOTEventAperture::particleTable = par->particleData;
#endif
    }
}

void DataLoader::BuildInputFileList(std::string inputPath)
{
  if (inputPath.empty())
    {throw RBDSException("DataLoader::BuildInputFileList> no file specified");}

  // wild card
  std::vector<std::string> fileNamesTemp;
  if (inputPath.find('*') != std::string::npos)
    {
      glob_t glob_result;
      glob(inputPath.c_str(),GLOB_TILDE,nullptr,&glob_result);
      for(unsigned int i=0;i<glob_result.gl_pathc;++i)
	{fileNamesTemp.emplace_back(glob_result.gl_pathv[i]);}
      globfree(&glob_result);
    }
  // single file
  else if (inputPath.find(".root") != std::string::npos)
    {fileNamesTemp.push_back(inputPath);}
  // directory
  else if (inputPath[inputPath.length()-1] == std::string("/"))
    {
      // find all files in directory
      inputPath.append("/*.root");
      
      glob_t glob_result;
      glob(inputPath.c_str(),GLOB_TILDE,nullptr,&glob_result);
      for (unsigned int i=0; i<glob_result.gl_pathc; ++i)
	{fileNamesTemp.emplace_back(glob_result.gl_pathv[i]);}
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
	  int value = fileDataVersion ? *fileDataVersion : -1;
	  std::cout << "Loading> \"" << fn << "\" : data version " << value << std::endl;
	  fileNames.push_back(fn);
	  dataVersion = fileDataVersion ? std::min(dataVersion, *fileDataVersion) : dataVersion;
	}
      else
	{std::cout << fn << " is not a BDSIM output file - skipping!" << std::endl;}
    }
  delete fileDataVersion;
  
  if (fileNames.empty())
    {throw RBDSException("DataLoader - No valid files found - check input file path / name");}
}

void DataLoader::BuildTreeNameList()
{
  // open file - this is the first opening so test here if it's valid
  TFile* f = new TFile(fileNames[0].c_str());
  if (f->IsZombie())
    {throw RBDSException(__METHOD_NAME__,"No such file \"" + fileNames[0] + "\"");}
  
  TList* kl = f->GetListOfKeys();
  for (int i = 0; i < kl->GetEntries(); ++i)
    {treeNames.emplace_back(std::string(kl->At(i)->GetName()));}

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
    {throw RBDSException(__METHOD_NAME__,"No such file \"" + fileNames[0] + "\"");}
  
  TTree* mt = (TTree*)f->Get("Model");
  if (!mt)
    {return;}

  Model* modTemporary = new Model(false, dataVersion);
  modTemporary->SetBranchAddress(mt);
  mt->GetEntry(0);
  allSamplerNames = modTemporary->SamplerNames();
  if (processSamplers)
    {samplerNames = allSamplerNames;} // copy sampler names out
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
  if (dataVersion > 6)
    {parChain->Add(fileNames[0].c_str());} // only require 1 copy
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
  if (dataVersion > 6)
    {par->SetBranchAddress(parChain);}
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
	for (const auto& bName : *evtBranches)
      {
        if (std::find(allSamplerNames.begin(), allSamplerNames.end(), bName + ".") != allSamplerNames.end())
          {samplerNames.push_back(bName + ".");}
      }
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
