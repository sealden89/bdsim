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
#ifndef HISTOGRAMMEANFROMFILE_H
#define HISTOGRAMMEANFROMFILE_H

#include <vector>

class HistogramAccumulator;

class BDSOutputROOTEventHistograms;

class TDirectory;

/**
 * @brief Accumulator to merge pre-made per-entry histograms.
 *
 * Operate on a stored series of histograms to merge them. Single
 * use only.
 * 
 * @author Stewart Boogert.
 */

class HistogramMeanFromFile
{
public:
  HistogramMeanFromFile(BDSOutputROOTEventHistograms* h);

  ~HistogramMeanFromFile();

  /// Add a new set of histograms to the running total. Assume
  /// exact same structure in BDSOutputROOTEventHistogams input.
  void Accumulate(BDSOutputROOTEventHistograms* hNew);

  /// Finish calculation.
  void Terminate();

  /// Write to file.
  void Write(TDirectory* dir = nullptr);

private:
  
  std::vector<HistogramAccumulator*> histograms1d;
  std::vector<HistogramAccumulator*> histograms2d;
  std::vector<HistogramAccumulator*> histograms3d;
};

#endif
