/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#ifndef BINSPECIFICATION_H
#define BINSPECIFICATION_H

#include <stdexcept>
#include <vector>

/**
 * @brief Binning specification for a single dimension.
 * 
 * For logarithmic or uneven binning, a set of bin edges can be
 * used. Otherwise, a high and low limit and the number of bins
 * in between can be used.
 *
 * "n" is the number of bins, not the number of edges.
 *
 * @author Laurie Nevay
 */

class BinSpecification
{
public:
  /// Default constructor.
  BinSpecification():
    low(0),
    high(1),
    n(1),
    edges(nullptr)
  {;}

  /// Linearly spaced bins.
  BinSpecification(double lowIn,
		   double highIn,
		   int    nIn):
    low(lowIn),
    high(highIn),
    n(nIn),
    edges(nullptr)
  {
    if (high <= low)
      {throw std::invalid_argument("high end of binning <= low end -> must be >");}
    if (n < 1)
      {throw std::invalid_argument("n bins < 1 -> must be >= 1");}
  }

  /// Uneven binning - suitable for logarithmic etc.
  explicit BinSpecification(const std::vector<double>& edgesIn):
    low(0),
    high(1),
    n(1),
    edges(new std::vector<double>(edgesIn))
  {
    if (edges->size() < 2)
      {throw std::invalid_argument("too few bin edges -> must be at least 2 edges to define 1 bin");}
    n    = (int)edges->size() - 1;
    low  = (*edges)[0];
    high = edges->back();
  }
  
  ~BinSpecification()
  {delete edges;}
  
  BinSpecification(const BinSpecification& other):
  low(other.low),
  high(other.high),
  n(other.n),
  edges(nullptr)
  {
    edges = other.edges ? new std::vector<double>(*other.edges) : nullptr;
  }
  
  BinSpecification(BinSpecification&& other) noexcept:
  low(other.low),
  high(other.high),
  n(other.n),
  edges(other.edges)
  {
    other.low   = 0;
    other.high  = 1;
    other.n     = 1;
    other.edges = nullptr;
  }
  
  double low;
  double high;
  int    n;
  std::vector<double>* edges;
};
  
#endif
