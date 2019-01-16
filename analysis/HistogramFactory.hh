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
#ifndef HISTOGRAMFACTORY_H
#define HISTOGRAMFACTORY_H

#include <string>

class HistogramDef;
class HistogramDef1D;
class HistogramDef2D;
class HistogramDef3D;

class TH1;
class TH1D;
class TH2D;
class TH3D;

namespace HistogramFactory
{
  /// Create a histogram of the appropriate dimensions. Forwards to
  /// other Ndim specific functions.
  TH1* CreateHistogram(const HistogramDef* definition,
		       std::string overRideName,
		       std::string overRideTitle);

  /// Whether name is set or not.
  void CheckNameAndTitle(std::string& name,
			 std::string& title,
			 const std::string& overRideName  = "",
			 const std::string& overRideTitle = "");

  /// Create 1D histogram.
  TH1D* CreateHistogram1D(const HistogramDef1D* d,
			  std::string overRideName  = "",
			  std::string overRideTitle = "");

  /// Create 2D histogram.
  TH2D* CreateHistogram2D(const HistogramDef2D* d,
			  std::string overRideName  = "",
			  std::string overRideTitle = "");

  /// Create 3D histogram.
  TH3D* CreateHistogram3D(const HistogramDef3D* d,
			  std::string overRideName  = "",
			  std::string overRideTitle = "");
}

#endif
