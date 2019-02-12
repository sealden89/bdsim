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
#include "HistogramDef1D.hh"

#include "TROOT.h"

ClassImp(HistogramDef1D)

HistogramDef1D::HistogramDef1D():
  xNBins(0),
  xLow(0),
  xHigh(0),
  logarithmicX(false)
{;}

HistogramDef1D::HistogramDef1D(std::string treeNameIn,
			       std::string histNameIn,
			       int         xNBinsIn,
			       double      xLowIn,
			       double      xHighIn,
			       std::string variableIn,
			       std::string selectionIn,
			       bool        perEntryIn,
			       bool        logarithmicXIn):
  HistogramDef(treeNameIn, histNameIn, 1, variableIn, selectionIn, perEntryIn),
  xNBins(xNBinsIn),
  xLow(xLowIn),
  xHigh(xHighIn),
  logarithmicX(logarithmicXIn)
{;}
