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
 * @file rebdsimCombine.cc
 */
#include "FileMapper.hh"
#include "HistogramAccumulatorMerge.hh"
#include "HistogramAccumulatorSum.hh"

#include "BDSOutputROOTEventHeader.hh"

#include "TDirectory.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
  if (argc < 3)
    {
      std::cout << "usage: rebdsimCombine result.root file1.root file2.root ..." << std::endl;
      exit(1);
    }

  std::string outputFile = std::string(argv[1]);
  // output file must be opened before histograms are created because root does
  // everything statically behind the scenes
  TFile* output = new TFile(outputFile.c_str(), "RECREATE");
  
  // add header for file type and version details
  output->cd();
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
  headerOut->Fill(); // updates time stamp
  headerOut->SetFileType("REBDSIMCOMBINE");
  TTree* headerTree = new TTree("Header", "REBDSIM Header");
  headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
  headerTree->Fill();
  output->Write(nullptr,TObject::kOverwrite);
  
  // build input file list
  std::vector<std::string> inputFiles;
  for (int i = 2; i < argc; ++i)
    {inputFiles.push_back(std::string(argv[i]));}
  
  // checks
  if (inputFiles.size() == 1)
    {
      std::cout << "Only one input file provided \"" << inputFiles[0] << "\" - no point." << std::endl;
      exit(1);
    }

  // ensure new histograms are written to file
  TH1::AddDirectory(true);
  TH2::AddDirectory(true);
  TH3::AddDirectory(true);

  TFile* f = nullptr; // temporary variable

  // initialise file map
  f = new TFile(inputFiles[0].c_str());
  HistogramMap* histMap = new HistogramMap(f, output); // map out first file
  f->Close();
  delete f;

  std::vector<RBDS::HistogramPath> histograms = histMap->Histograms();

  // loop over files and accumulate
  for (const auto& file : inputFiles)
    {
      f = new TFile(file.c_str());
      if (RBDS::IsREBDSIMOrCombineOutputFile(f))
	{
	  std::cout << "Accumulating> " << file << std::endl;
	  for (const auto& hist : histograms)
	    {
	      std::string histPath = hist.path + hist.name; // histPath has trailing '/'
	      TH1* h = static_cast<TH1*>(f->Get(histPath.c_str()));
	      if (!h)
		{RBDS::WarningMissingHistogram(histPath, file); continue;}
	      hist.accumulator->Accumulate(h);
	    }
	}
      else
	{std::cout << "Skipping " << file << " as not a rebdsim output file" << std::endl;}
      f->Close();
      delete f;
    }
  
  // terminate and write output
  for (const auto& hist : histograms)
    {
      TH1* result = hist.accumulator->Terminate();
      result->SetDirectory(hist.outputDir);
      hist.outputDir->Add(result);
      delete hist.accumulator; // this removes temporary histograms from the file
    }

  output->Write(nullptr,TObject::kOverwrite);
  output->Close();
  delete output;
  
  return 0;
}
