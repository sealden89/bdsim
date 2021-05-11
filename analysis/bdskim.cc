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
/**
 * @file bdskim.cc
 */
#include "FileMapper.hh"
#include "Header.hh"
#include "SelectionLoader.hh"

#include "BDSOutputROOTEventHeader.hh"

#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
  if (argc != 4)
    {
      std::cout << "usage: bdskim skimselection.txt input_bdsim_raw.root output_bdsim_raw.root" << std::endl;
      return 1;
    }

  std::string selectionFile = std::string(argv[1]);
  std::string inputFile     = std::string(argv[2]);
  std::string outputFile    = std::string(argv[3]);

  // load selection
  std::string selection;
  try
    {selection = RBDS::LoadSelection(selectionFile);}
  catch (std::exception& e)
    {std::cerr << e.what() << std::endl; return 1;}

  // open and check input file
  TFile* input = new TFile(inputFile.c_str(),  "READ");
  if (!RBDS::IsBDSIMOutputFile(input))
    {
      std::cerr << inputFile << " is not a BDSIM output file" << std::endl;
      delete input;
      return 1;
    }
  
  // note we create trees in the new output file in order the same as the original to preserve the 'look' of the file
 
  TTree* headerTree = dynamic_cast<TTree*>(input->Get("Header")); // should be safe given check we've just done
  if (!headerTree)
    {std::cerr << "Error with header" << std::endl; exit(1);}
  Header* headerLocal = new Header();
  headerLocal->SetBranchAddress(headerTree);
  headerTree->GetEntry(0);
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader(*(headerLocal->header));
  headerOut->skimmedFile = true;
  
  TFile* output = new TFile(outputFile.c_str(), "RECREATE");
  if (!output)
    {std::cerr << "Couldn't open output file " << outputFile << std::endl; exit(1);}
  output->cd();
  TTree* outputHeaderTree = new TTree("Header", "BDSIM Header");
  outputHeaderTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);

  std::vector<std::string> treeNames = {"ParticleData", "Beam", "Options", "Model", "Run"};
  for (const auto& tn : treeNames)
    {
      TTree* original = dynamic_cast<TTree*>(input->Get(tn.c_str()));
      if (!original)
	{
	  std::cerr << "Failed to load Tree named " << tn << std::endl;
	  delete output;
	  delete input;
	  return 1;
	}
      auto clone = original->CloneTree();
      clone->AutoSave();
    }
  
  TTree* allEvents = dynamic_cast<TTree*>(input->Get("Event"));
  if (!allEvents)
    {
      std::cerr << "No Event tree in file" << std::endl;
      delete input;
      return 1;
    }
  TTree* selectEvents = allEvents->CopyTree(selection.c_str());
  selectEvents->Write();
  
  headerOut->nOriginalEvents = allEvents->GetEntries(); // update original number of entries
  outputHeaderTree->Fill();
  output->Write(nullptr,TObject::kOverwrite);
  delete output;
  delete input;
  
  return 0;
}
