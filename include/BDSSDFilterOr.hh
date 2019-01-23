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
#ifndef BDSSDFILTEROR_H
#define BDSSDFILTEROR_H

#include "G4String.hh"
#include "G4Types.hh"
#include "G4VSDFilter.hh"

#include <vector>

/**
 * @brief Filter that applies or to a vector of filters.
 * 
 * Doesn't own the underlying filters.
 * 
 * @author Laurie Nevay
 */

class BDSSDFilterOr: public G4VSDFilter
{
public:
  BDSSDFilterOr(const G4String& name);
  virtual ~BDSSDFilterOr(){;}

  /// Return true if the particle is a primary. If no filters are registered,
  /// it will return false and not accept anything.
  virtual G4bool Accept(const G4Step* step) const;

  /// Register the filter.
  inline void RegisterFilter(G4VSDFilter* filterIn) {filters.push_back(filterIn);}

  /// Accessor.
  inline const std::vector<G4VSDFilter*>& Filters() const {return filters;}

private:
  BDSSDFilterOr() = delete;

  /// Vector of all filters to be 'or'd'.
  std::vector<G4VSDFilter*> filters;
};

#endif
