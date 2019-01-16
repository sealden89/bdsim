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
#include "HistogramDef2D.hh"


HistogramDef2D::HistogramDef2D(std::string treeNameIn,
			       std::string histNameIn,
			       int         xNBinsIn,
			       int         yNBinsIn,
			       double      xLowIn,
			       double      xHighIn,
			       double      yLowIn,
			       double      yHighIn,
			       std::string plotIn,
			       std::string selectionIn,
			       bool        perEntryIn,
			       bool        logarithmicXIn,
			       bool        logarithmicYIn):
  HistogramDef1D(treeNameIn, histNameIn, xNBinsIn, xLowIn,
		 xHighIn, plotIn, selectionIn, perEntryIn, logarithmicXIn),
  yNBins(yNBinsIn),
  yLow(yLowIn),
  yHigh(yHighIn),
  logarithmicY(logarithmicYIn)
{
  nDimensions = 2;
}
