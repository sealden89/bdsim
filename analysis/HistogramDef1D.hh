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
#ifndef HISTOGRAMDEF1D_H
#define HISTOGRAMDEF1D_H

#include "HistogramDef.hh"

#include <string>

#include "Rtypes.h"

/**
 * @brief Specification for 1D histogram.
 *
 * @author L. Nevay
 */

class HistogramDef1D: public HistogramDef
{
public:
  /// Public constructor only for compatibility with ROOT - not indended for use.
  HistogramDef1D();

  /// Use this constructor.
  HistogramDef1D(std::string treeNameIn,
		 std::string histNameIn,
		 int         xNBinsIn,
		 double      xLowIn,
		 double      xHighIn,
		 std::string variableIn,
		 std::string selectionIn  = "1",
		 bool        perEntryIn   = true,
		 bool        logarithmicX = false);
  virtual ~HistogramDef1D(){;}
  
  int         xNBins;
  double      xLow;
  double      xHigh;
  bool        logarithmicX;

  ClassDef(HistogramDef1D, 1);
};

#endif
