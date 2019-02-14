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
#ifndef HISTOGRAMDEF3D_H
#define HISTOGRAMDEF3D_H

#include "HistogramDef2D.hh"

#include <string>

#include "TROOT.h"

/**
 * @brief Specification for 3D Histogram.
 *
 * @author L. Nevay
 */

class HistogramDef3D: public HistogramDef2D
{
public:
  /// Public constructor only for compatibility with ROOT - not indended for use.
  HistogramDef3D();

  /// Use this constructor.
  HistogramDef3D(std::string treeNameIn,
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
		 std::string selectionIn    = "1",
		 bool        perEntryIn     = true,
		 bool        logarithmicXIn = false,
		 bool        logarithmicYIn = false,
		 bool        logarithmicZIn = false);
  virtual ~HistogramDef3D(){;}

  int         zNBins;
  double      zLow;
  double      zHigh;
  bool        logarithmicZ;

  ClassDef(HistogramDef3D, 1);
};

#endif
