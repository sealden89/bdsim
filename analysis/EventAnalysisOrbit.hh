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
#ifndef EVENTANALYSISORBIT_H
#define EVENTANALYSISORBIT_H

#include "EventAnalysis.hh"

class TFile;

/**
 * @brief Simple analysis to pull out first hit in each sampler.
 *
 * @author L. Nevay
 */

class EventAnalysisOrbit: public EventAnalysis
{
public:
  using EventAnalysis::EventAnalysis;

  virtual ~EventAnalysisOrbit(){;}

  void Clear();                 ///< Empty the member vectors of their data.
  void ExtractOrbit(int index); ///< Extract an orbit from the data.
  void WriteOrbit(TFile* f);    ///< Write orbit to a ROOT file.

  /// @{ Temporary storage for orbit.
  std::vector<double> ss;
  std::vector<double> x;
  std::vector<double> xp;
  std::vector<double> y;
  std::vector<double> yp;
  std::vector<std::string> elementName;
  /// @}
};

#endif
