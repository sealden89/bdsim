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
/**
 * @file rebdsim.cc
 */

#include <iostream>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include "BDSOutputROOTEventHeader.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventModel.hh"

#include "Analysis.hh"
#include "Config.hh"
#include "DataLoader.hh"
#include "BeamAnalysis.hh"
#include "EventAnalysis.hh"
#include "ModelAnalysis.hh"
#include "OptionsAnalysis.hh"
#include "RebdsimTypes.hh"
#include "RunAnalysis.hh"

int main(int argc, char *argv[])
{
  // check input
  if (argc != 3)
    {
      std::cout << "usage: rebdsim <dataFile> <outputFile>" << std::endl;
      std::cout << " <datafile> - root file to operate on" << std::endl;
      std::cout << " <outputfile> - output file name for analysis" << std::endl;
      exit(1);
    }
  
  std::string inputFilePath = std::string(argv[1]);
  std::string outputFileName = std::string(argv[2]);

  // Setup config
  Config* config = Config::Instance("", inputFilePath, outputFileName);
  
  bool allBranches = config->AllBranchesToBeActivated();
  const RBDS::BranchMap* branchesToActivate = &(config->BranchesToBeActivated());
  
  bool debug = config->Debug();
  DataLoader* dl = nullptr;
  try
    {
      dl = new DataLoader(config->InputFilePath(),
			  debug,
			  config->ProcessSamplers(),
			  allBranches,
			  branchesToActivate,
			  config->GetOptionBool("backwardscompatible"));
    }
  catch (const std::string& e)
    {std::cerr << e << std::endl; exit(1);}
  
  BeamAnalysis*    beaAnalysis = new BeamAnalysis(dl->GetBeam(),
						  dl->GetBeamTree(),
						  config->PerEntryBeam(),
						  debug);
  EventAnalysis*   evtAnalysis = new EventAnalysis(dl->GetEvent(),
                                                   dl->GetEventTree(),
						   config->PerEntryEvent(),
						   config->ProcessSamplers(),
                                                   debug,
                                                   config->PrintModuloFraction(),
						   config->GetOptionBool("emittanceonthefly"));
  RunAnalysis*     runAnalysis = new RunAnalysis(dl->GetRun(),
						 dl->GetRunTree(),
						 config->PerEntryRun(),
						 debug);
  OptionsAnalysis* optAnalysis = new OptionsAnalysis(dl->GetOptions(),
						     dl->GetOptionsTree(),
						     config->PerEntryOption(),
						     debug);
  ModelAnalysis*   modAnalysis = new ModelAnalysis(dl->GetModel(),
						   dl->GetModelTree(),
						   config->PerEntryModel(),
						   debug);

  std::vector<Analysis*> analyses = {beaAnalysis,
				     evtAnalysis,
				     runAnalysis,
				     optAnalysis,
				     modAnalysis};

  for (auto& analysis : analyses)
    {analysis->Execute();}

  // write output
  try
    {
      TFile* outputFile = new TFile(config->OutputFileName().c_str(),"RECREATE");

      // add header for file type and version details
      outputFile->cd();
      BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
      headerOut->Fill(dl->GetFileNames()); // updates time stamp
      headerOut->SetFileType("REBDSIM");
      TTree* headerTree = new TTree("Header", "REBDSIM Header");
      headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
      headerTree->Fill();
      headerTree->Write("", TObject::kOverwrite);
      
      for (auto& analysis : analyses)
	{analysis->Write(outputFile);}

      // copy the model over and rename to avoid conflicts with Model directory
      auto modelTree = dl->GetModelTree();
      auto newTree = modelTree->CloneTree();
      // unforunately we have a folder called Model in histogram output files
      // avoid conflict when copying the model for plotting
      newTree->SetName("ModelTree");
      newTree->Write("", TObject::kOverwrite);

      outputFile->Close();
      delete outputFile;
      std::cout << "Result written to: " << config->OutputFileName() << std::endl;
    }
  catch (const std::string& error)
    {
      std::cout << error << std::endl;
      exit(1);
    }
  delete dl;
  return 0;
}
