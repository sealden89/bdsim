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
#ifndef HISTOGRAMDEF_H
#define HISTOGRAMDEF_H

#include <string>

/**
 * @brief Common specification for a histogram.
 *
 * @author L. Nevay
 */

class HistogramDef
{
public:
  HistogramDef(std::string treeNameIn,
	       std::string histNameIn,
	       int         nDimensionsIn,
               std::string variable,
	       std::string selectionIn = "1",
	       bool        perEnetry   = true);
  virtual ~HistogramDef(){;}
  
  std::string treeName;
  std::string histName;
  int         nDimensions;
  std::string variable;
  std::string selection;
  bool        perEntry;

private:
  /// No default constructor
  HistogramDef() = delete;
};

#endif
