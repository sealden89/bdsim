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
#ifndef HISTOGRAMACCUMULATORSUM_H
#define HISTOGRAMACCUMULATORSUM_H

#include "HistogramAccumulator.hh"

#include <string>

#include "Rtypes.h" // for classdef

class TH1;

/**
 * @brief Class to Sum histogram.
 *
 * This overrides two methods for the accumulation but the inheritance
 * saves on repetition in the constructor and structures.
 * 
 * @author Laurie Nevay
 */

class HistogramAccumulatorSum: public HistogramAccumulator
{
public:
  /// Public constructor only for compatibility with ROOT - not indended for use.
  HistogramAccumulatorSum();
  
  /// Constructor simply passes down to base class.
  HistogramAccumulatorSum(TH1*               baseHistogram,
			  const int&         nDimensionsIn,
			  const std::string  resultHistName,
			  const std::string& resultHistTitle);
  
  virtual ~HistogramAccumulatorSum(){;}

  /// This method must be overridden as the information passed to the
  /// AccumualteSingleValue is different.
  virtual void Accumulate(TH1* newValue);

protected:

  /// This implements a different method from the base class that is used
  /// to combine means and variances from different samples of the same underlying
  /// experiment / data.
  virtual void AccumulateSingleValue(const double& oldMean,
				     const double& oldVari,
				     const double& x,
				     const double& xVari,
				     const unsigned long& nEntriesAccumulated,
				     const unsigned long& nEntriesToAccumulate,
				     double&       newMean,
				     double&       newVari) const;

  ClassDef(HistogramAccumulatorSum, 1);
};

#endif
