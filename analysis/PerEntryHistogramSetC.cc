/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "Event.hh"
#include "PerEntryHistogramSetC.hh"

#include "BDSOutputROOTEventSamplerC.hh"
#include "RBDSException.hh"

#include <set>
#include <string>
#include <vector>


PerEntryHistogramSetC::PerEntryHistogramSetC(const HistogramDefSet* definitionIn,
                                             Event*                 eventIn,
                                             TChain*                chainIn):
  PerEntryHistogramSet(definitionIn, eventIn, chainIn),
  sampler(nullptr)
{;}

PerEntryHistogramSetC::~PerEntryHistogramSetC()
{;}

void PerEntryHistogramSetC::CheckSampler()
{
  if (!sampler)
    {
      sampler = event->GetSamplerC(branchName + "."); // cache sampler
      if (!sampler)
        {throw RBDSException("Cannot find csampler \"" + branchName + "\" or Model tree was not stored");}
    }
}

void PerEntryHistogramSetC::GetPDGIDSetFromSampler(std::set<long long int>& setIn) const
{
  setIn.insert(sampler->partID.begin(), sampler->partID.end());
}
