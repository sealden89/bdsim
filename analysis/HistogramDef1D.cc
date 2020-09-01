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
#include "BinSpecification.hh"
#include "HistogramDef1D.hh"

ClassImp(HistogramDef1D)

HistogramDef1D::HistogramDef1D():
  xBinning(BinSpecification())
{;}

HistogramDef1D::HistogramDef1D(const std::string& treeNameIn,
			       const std::string& histNameIn,
			       const BinSpecification& xBinningIn,
			       const std::string& variableIn,
			       const std::string& selectionIn,
			       bool               perEntryIn):
  HistogramDef(treeNameIn, histNameIn, 1, variableIn, selectionIn, perEntryIn),
  xBinning(xBinningIn)
{;}

HistogramDef1D::~HistogramDef1D()
{;}
