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
#ifndef PERENTRYHISTOGRAM_H
#define PERENTRYHISTOGRAM_H

#include <string>

#include "TROOT.h"

class HistogramAccumulator;
class HistogramDef;

class TChain;
class TDirectory;
class TH1;

/**
 * @brief Holder for information to calculate per entry histograms.
 *
 * This creates a histogram per event and accumualtes the mean and variance
 * for every bin in a cloned histogram. In the case of 1 event, the bin error
 * is 0. 
 * 
 * This uses a HistogramAccumulator object rather than inheritance as this
 * class has to prepare the base histogram in the constructor first.
 * 
 * @author Laurie Nevay
 */

class PerEntryHistogram
{
public:
  /// Public constructor only for compatibility with ROOT - not indended for use.
  PerEntryHistogram();
  
  /// Constructor with a histogram definition and the chain to operate on.
  PerEntryHistogram(const HistogramDef* definition,
		    TChain*             chain);
  virtual ~PerEntryHistogram();

  /// Create a histogram of the approprate dimensions for the currently loaded
  /// event then add it to the online (ie running) means and variances.
  virtual void AccumulateCurrentEntry(const long int& entryNumber);

  /// Terminate the accumulator and save the result to the result member variable.
  void Terminate();

  /// Forwarding function - call Write on result histograms on the currently
  /// open file. Optional directory to specify where the histogram should be moved to.
  void Write(TDirectory* dir = nullptr);

protected:
  HistogramAccumulator* accumulator;
  TChain*       chain;        ///< Cache of chain pointer that provides data.
  std::string   selection;    ///< Selection command.
  TH1*          temp;         ///< Histogram for temporary 1 event data.
  TH1*          result;       ///< Final result with errors as the error on the mean.
  std::string   command;      ///< Draw command.
  
  ClassDef(PerEntryHistogram, 1);
};

#endif
