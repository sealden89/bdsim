/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#include "Beam.hh"
#include "BeamAnalysis.hh"

#include "Rtypes.h" // for classdef

class TChain;

ClassImp(BeamAnalysis)

BeamAnalysis::BeamAnalysis():
  Analysis("Beam.", nullptr, "BeamHistogramsMerged"),
  beam(nullptr)
{;}

BeamAnalysis::BeamAnalysis(Beam*   beamIn,
			   TChain* chainIn,
			   bool    perEntryAnalysis,
			   bool    debugIn):
  Analysis("Beam.", chainIn, "BeamHistogramsMerged", perEntryAnalysis, debugIn),
  beam(beamIn)
{;}

BeamAnalysis::~BeamAnalysis() noexcept
{;}

void BeamAnalysis::Process()
{
  for (int i = 0; i < entries; i++)
    {
      // per event histograms
      AccumulatePerEntryHistograms(i);
    }
}
