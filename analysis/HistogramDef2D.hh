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
#ifndef HISTOGRAMDEF2D_H
#define HISTOGRAMDEF2D_H

#include "HistogramDef1D.hh"

#include <string>

/**
 * @brief Specification for 2D histogram.
 *
 * @author L. Nevay
 */

class HistogramDef2D: public HistogramDef1D
{
public:
  HistogramDef2D(std::string treeNameIn,
		 std::string histNameIn,
		 int         xNBinsIn,
		 int         yNBinsIn,
		 double      xLowIn,
		 double      xHighIn,
		 double      yLowIn,
		 double      yHighIn,
		 std::string plotIn,
		 std::string selectionIn    = "1",
		 bool        perEntryIn     = true,
		 bool        logarithmicXIn = false,
		 bool        logarithmicYIn = false);
  virtual ~HistogramDef2D(){;}

  int         yNBins;
  double      yLow;
  double      yHigh;
  bool        logarithmicY;
  
private:
  /// No default constructor
  HistogramDef2D() = delete;
};

#endif
