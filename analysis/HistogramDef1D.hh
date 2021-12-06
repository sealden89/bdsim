/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BinSpecification.hh"
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
  /// Public constructor only for compatibility with ROOT - not intended for use.
  HistogramDef1D();

  /// Use this constructor.
  HistogramDef1D(const std::string&      treeNameIn,
		 const std::string&      histNameIn,
		 const BinSpecification& xBinningIn,
		 const std::string&      variableIn,
		 const std::string&      selectionIn = "1",
		 bool                    perEntryIn  = true);
  
  virtual ~HistogramDef1D();
    
  /// Copy this instance. Virtual to be overridden in derived classes.
  virtual HistogramDef* Clone() const {return new HistogramDef1D(*this);}

  BinSpecification xBinning;

  ClassDef(HistogramDef1D, 1);
};

#endif
