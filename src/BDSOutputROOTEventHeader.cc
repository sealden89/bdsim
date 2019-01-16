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
#include "BDSOutputROOTEventHeader.hh"
#include "version.h"

#include "G4Version.hh"

#include "TROOT.h"

#include "CLHEP/ClhepVersion.h"

#include <ctime>
#include <string>

ClassImp(BDSOutputROOTEventHeader)

BDSOutputROOTEventHeader::BDSOutputROOTEventHeader()
{
  Flush();
}

BDSOutputROOTEventHeader::~BDSOutputROOTEventHeader()
{;}

void BDSOutputROOTEventHeader::Flush()
{
  bdsimVersion  = std::string(GIT_VERSION);
  geant4Version = G4Version;
  rootVersion   = std::string(gROOT->GetVersion());
  clhepVersion  = CLHEP::Version::String();
  timeStamp     = "";
  fileType      = "BDSIM";
  dataVersion   = 4; // update analysis/DataLoader.cc default when this changes
  
#ifndef __ROOTDOUBLE__
  doublePrecisionOutput = false;
#else
  doublePrecisionOutput = true;
#endif
}

void BDSOutputROOTEventHeader::Fill()
{
  time_t rawtime;
  time(&rawtime);
  timeStamp = std::string(ctime(&rawtime));
}
  
