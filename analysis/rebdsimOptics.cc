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
/**
 * @file rebdsimOptics.cc
 */

#include <iostream>
#include <string>

#include "DataLoader.hh"
#include "EventAnalysis.hh"

#include "BDSOutputROOTEventHeader.hh"

#include "TFile.h"
#include "TChain.h"

void usage()
{ 
  std::cout << "usage: rebdsimOptics <dataFile> <outputfile> <--emittanceOnFly>" << std::endl;
  std::cout << " <datafile>   - root file to operate on ie run1.root"            << std::endl;
  std::cout << " <outputfile> - name of output file ie optics.dat"               << std::endl;
  std::cout << " --emittanceOnFly - calculate emittance per sampler (optional)"  << std::endl;
  std::cout << "Quotes should be used if * is used in the input file name."      << std::endl;
}

int main(int argc, char* argv[])
{
  if (argc < 3 || argc > 4)
    {
      std::cout << "Incorrect number of arguments." << std::endl;
      usage();
      exit(1);
    }

  std::string inputFileName   = std::string(argv[1]);
  std::string outputFileName  = std::string(argv[2]);

  bool emittanceOnFly = false;
  if (argc == 4)
    {
      std::string emittanceOnFlyS = std::string(argv[3]);
      if (emittanceOnFlyS == "--emittanceOnFly")
	{
	  emittanceOnFly = true;
	  std::cout << "Calculating emittance per sampler" << std::endl;
	}
    }

  DataLoader dl = DataLoader(inputFileName, false, true);
  EventAnalysis* evtAnalysis = new EventAnalysis(dl.GetEvent(), dl.GetEventTree(),
						 false, true, false, -1, emittanceOnFly);
  evtAnalysis->Execute();

  TFile* outputFile = new TFile(outputFileName.c_str(), "RECREATE");

  // add header for file type and version details
  outputFile->cd();
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
  headerOut->Fill(); // updates time stamp
  headerOut->SetFileType("REBDSIM");
  TTree* headerTree = new TTree("Header", "REBDSIM Header");
  headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
  headerTree->Fill();
  headerTree->Write("", TObject::kOverwrite);

  // write merged histograms and optics
  evtAnalysis->Write(outputFile);

  // clone model tree for nice built in optics plotting
  auto modelTree = dl.GetModelTree();
  auto newTree   = modelTree->CloneTree();
  newTree->Write("", TObject::kOverwrite);
  outputFile->ls();
  
  outputFile->Close();
  delete outputFile;
  
  return 0;
}
