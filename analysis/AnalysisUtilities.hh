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
#ifndef ANALYSISUTILITIES_H
#define ANALYSISUTILITIES_H

#include "Rtypes.h" // for classdef

#include <string>
#include <vector>

class HistogramDef;
class HistogramDef1D;
class HistogramDef2D;
class HistogramDef3D;

class TH1;
class TH1D;
class TH2D;
class TH3D;

/**
 * @brief Simple class to hold all unbound functions for ROOT.
 *
 * ROOT struggles to generate dictionaries for namespaced functions
 * and global functions. Here, we have a simple class with no members
 * that has the functions required.
 *
 * @author Laurie Nevay.
 */

class AnalysisUtilities
{
public:
  AnalysisUtilities();
  virtual ~AnalysisUtilities();

  /* @brief Method to generate evenly spaced points on a log scale.
   * 
   * Based on solution on StackOverFlow:
   * https://stackoverflow.com/a/21429452
   */
  std::vector<double> LogSpace(double start,
			       double stop,
			       int    nBins,
			       double base = 10,
			       bool   includeLastPoint = true);

  /// Linear range of values.
  std::vector<double> LinSpace(double start,
			       double stop,
			       int    nBins,
			       bool   includeLastPoint = true);
  
  ClassDef(AnalysisUtilities,1);
};

#endif
