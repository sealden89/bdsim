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
#include "Analysis.hh"
#include "BinGeneration.hh"
#include "Config.hh"
#include "HistogramDef.hh"
#include "HistogramDef1D.hh"
#include "HistogramDef2D.hh"
#include "HistogramDef3D.hh"
#include "HistogramFactory.hh"
#include "HistogramMeanFromFile.hh"
#include "PerEntryHistogram.hh"
#include "rebdsim.hh"

#include "TChain.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

#include <iostream>
#include <string>
#include <vector>

Analysis::Analysis(const std::string& treeNameIn,
		   TChain*     chainIn,
		   const std::string& mergedHistogramNameIn,
		   bool        perEntryAnalysis,
		   bool        debugIn):
  treeName(treeNameIn),
  chain(chainIn),
  mergedHistogramName(mergedHistogramNameIn),
  histoSum(nullptr),
  debug(debugIn),
  entries(chain->GetEntries()),
  perEntry(perEntryAnalysis)
{;}

Analysis::~Analysis()
{
  delete chain;
  delete histoSum;
  for (auto pe : perEntryHistograms)
    {delete pe;}
}

void Analysis::Execute()
{
  std::cout << "Analysis on \"" << treeName << "\" beginning" << std::endl;
  if (perEntry)
    {
      // ensure new histograms are added to file
      // crucial for draw command to work as it identifies the histograms by name
      TH1::AddDirectory(kTRUE);
      TH2::AddDirectory(kTRUE);
      TH3::AddDirectory(kTRUE);
      PreparePerEntryHistograms();
      Process();
    }
  SimpleHistograms();
  Terminate();
  std::cout << "Analysis on \"" << treeName << "\" complete" << std::endl;
}

void Analysis::UserProcess()
{;}

void Analysis::SimpleHistograms()
{
  // loop over histogram specifications and fill
  // TBC - in future we should avoid the singleton accessor as rebdsimOptics
  // doesn't use it but uses the event analysis.
  auto c = Config::Instance();
  if (c)
    {
      auto definitions = Config::Instance()->HistogramDefinitionsSimple(treeName);
      for (auto definition : definitions)
	{FillHistogram(definition);}
    }
}

void Analysis::PreparePerEntryHistograms()
{
  auto c = Config::Instance();
  if (c)
    {
      auto definitions = c->HistogramDefinitionsPerEntry(treeName);
      for (const auto &def : definitions)
        {perEntryHistograms.push_back(new PerEntryHistogram(def, chain));}
    }
}

void Analysis::AccumulatePerEntryHistograms(const long int& entryNumber)
{
  auto c = Config::Instance();
  if (c)
    {
      auto definitions = c->HistogramDefinitionsPerEntry(treeName);
      for (auto &peHist : perEntryHistograms)
        {peHist->AccumulateCurrentEntry(entryNumber);}
    }
}

void Analysis::TerminatePerEntryHistograms()
{
  auto c = Config::Instance();
  if (c)
    {
      auto definitions = c->HistogramDefinitionsPerEntry(treeName);
      for (auto &peHist : perEntryHistograms)
        {peHist->Terminate();}
    }
}

void Analysis::Terminate()
{
  if (histoSum)
    {histoSum->Terminate();}
  if (perEntry)
    {TerminatePerEntryHistograms();}
}

void Analysis::Write(TFile* outputFile)
{
  // treeName typically has a "." at the end, deleting it here:
  std::string cleanedName     = treeName.erase(treeName.size() - 1);
  std::string perEntryDirName = "PerEntryHistograms";
  std::string simpleDirName   = "SimpleHistograms";
  std::string mergedDirName   = "MergedHistograms";
  TDirectory* rebdsimDir  = outputFile->mkdir(cleanedName.c_str());
  TDirectory* perEntryDir = rebdsimDir->mkdir(perEntryDirName.c_str());
  TDirectory* simpleDir   = rebdsimDir->mkdir(simpleDirName.c_str());
  TDirectory* mergedDir   = rebdsimDir->mkdir(mergedDirName.c_str());

  perEntryDir->cd();
  for (auto h : perEntryHistograms)
    {h->Write(perEntryDir);}

  // simple histograms
  simpleDir->cd();
  for (auto& h : histograms1D)
    {simpleDir->Add(h.second);}
  for (auto& h : histograms2D)
    {simpleDir->Add(h.second);}
  for (auto& h : histograms3D)
    {simpleDir->Add(h.second);}
  for (auto& h : histograms1D)
    {h.second->Write();}
  for (auto& h : histograms2D)
    {h.second->Write();}
  for (auto& h : histograms3D)
    {h.second->Write();}

  // merged histograms
  if (histoSum)
    {
      mergedDir->cd();
      std::cout << "Merging histograms from \"" << treeName << "\" analysis" << std::endl;
      histoSum->Write(mergedDir);
    }

  outputFile->cd("/");  // return to root of the file
}

void Analysis::FillHistogram(HistogramDef* definition)
{
  // ensure new histograms are added to file..
  // this is crucial for the draw command to work as it finds the histograms by name
  TH1::AddDirectory(kTRUE);
  TH2::AddDirectory(kTRUE);
  TH3::AddDirectory(kTRUE);
  
  // pull out communal information in base class
  int         nDim      = definition->nDimensions;
  std::string name      = definition->histName;
  std::string command   = definition->variable + " >> " + definition->histName;
  std::string selection = definition->selection;
  
  switch (nDim)
    {
    case 1:
      {
	HistogramDef1D* d = dynamic_cast<HistogramDef1D*>(definition);
	TH1D* h = HistogramFactory::CreateHistogram1D(d);
	chain->Draw(command.c_str(), selection.c_str(),"goff");
	histogramNames.push_back(name);
	histograms1D[name] = h;
	break;
      }
    case 2:
      {
	HistogramDef2D* d = dynamic_cast<HistogramDef2D*>(definition);
	TH2D* h = HistogramFactory::CreateHistogram2D(d);
	chain->Draw(command.c_str(), selection.c_str(),"goff");
	histogramNames.push_back(name);
	histograms2D[name] = h;
	break;
      }
    case 3:
      {
	HistogramDef3D* d = dynamic_cast<HistogramDef3D*>(definition);
	TH3D* h = HistogramFactory::CreateHistogram3D(d);
	chain->Draw(command.c_str(), selection.c_str(),"goff");
	histogramNames.push_back(name);
	histograms3D[name] = h;
	break;
      }
    default:
      {break;}
    }
}

