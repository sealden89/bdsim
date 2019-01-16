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
#include "HistogramDef3D.hh"


HistogramDef3D::HistogramDef3D(std::string treeNameIn,
			       std::string histNameIn,
			       int         xNBinsIn,
			       int         yNBinsIn,
			       int         zNBinsIn,
			       double      xLowIn,
			       double      xHighIn,
			       double      yLowIn,
			       double      yHighIn,
			       double      zLowIn,
			       double      zHighIn,
			       std::string plotIn,
			       std::string selectionIn,
			       bool        perEntryIn,
			       bool        logarithmicXIn,
			       bool        logarithmicYIn,
			       bool        logarithmicZIn):
  HistogramDef2D(treeNameIn, histNameIn, xNBinsIn, yNBinsIn,
		 xLowIn, xHighIn, yLowIn, yHighIn,
		 plotIn, selectionIn, perEntryIn,
		 logarithmicXIn, logarithmicYIn),
  zNBins(zNBinsIn),
  zLow(zLowIn),
  zHigh(zHighIn),
  logarithmicZ(logarithmicZIn)
{
  nDimensions = 3;
}
