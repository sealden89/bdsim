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
#include "BDSOutputROOTEventHeader.hh"
#include "BDSTrajectoryFilter.hh"       // no G4 types and for size of filters
#include "BDSVersion.hh"

#include "G4Version.hh"

#include "TROOT.h"

#include "CLHEP/ClhepVersion.h"

#include <ctime>
#include <string>
#include <vector>

ClassImp(BDSOutputROOTEventHeader)

BDSOutputROOTEventHeader::BDSOutputROOTEventHeader()
{
  Flush();
}

BDSOutputROOTEventHeader::~BDSOutputROOTEventHeader()
{;}

void BDSOutputROOTEventHeader::FlushLocal()
{
  bdsimVersion  = std::string(BDSIM_GIT_VERSION);
  geant4Version = G4Version;
  rootVersion   = std::string(gROOT->GetVersion());
  clhepVersion  = CLHEP::Version::String();
  timeStamp     = "";
  fileType      = "BDSIM";
  dataVersion   = 5; // update analysis/DataLoader.cc default when this changes
  // also in comparator/Compare.cc at top - EXPECTEDDATAVERSION
  analysedFiles.clear();
  combinedFiles.clear();
  nTrajectoryFilters = BDS::NTrajectoryFilters;
  trajectoryFilters.clear();
  
#ifndef __ROOTDOUBLE__
  doublePrecisionOutput = false;
#else
  doublePrecisionOutput = true;
#endif
}

void BDSOutputROOTEventHeader::Fill(const std::vector<std::string>& analysedFilesIn,
				    const std::vector<std::string>& combinedFilesIn)
{
  time_t rawtime;
  time(&rawtime);
  timeStamp = std::string(ctime(&rawtime));
  analysedFiles = analysedFilesIn;
  combinedFiles = combinedFilesIn;
#ifndef __ROOTBUILD__
  FillGeant4Side();
#endif
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventHeader::FillGeant4Side()
{
  for (int i = 0; i < nTrajectoryFilters; i++)
    {trajectoryFilters.push_back(BDS::BDSTrajectoryFilterEnumOfIndex(i).ToString());}
}
#endif
