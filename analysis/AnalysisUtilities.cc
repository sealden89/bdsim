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
#include "AnalysisUtilities.hh"
#include "BinGeneration.hh"

#include "Rtypes.h" // for classdef

ClassImp(AnalysisUtilities)

AnalysisUtilities::AnalysisUtilities()
{;}

AnalysisUtilities::~AnalysisUtilities()
{;}

std::vector<double> AnalysisUtilities::LogSpace(double start,
						double stop,
						int    nBins,
						double base,
						bool   includeLastPoint)
{
  return RBDS::LogSpace(start, stop, nBins, base, includeLastPoint);
}

std::vector<double> AnalysisUtilities::LinSpace(double start,
						double stop,
						int    nBins,
						bool   includeLastPoint)
{
  return RBDS::LinSpace(start, stop, nBins, includeLastPoint);
}
