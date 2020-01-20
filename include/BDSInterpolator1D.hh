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
#ifndef BDSINTERPOLATOR1D_H
#define BDSINTERPOLATOR1D_H

#include "BDSArray1DCoords.hh"
#include "BDSDimensionType.hh"
#include "BDSFieldValue.hh"

#include "G4Types.hh"
#include "G4ThreeVector.hh"

/**
 * @brief Interface for all 1D interpolators.
 *
 * Does not own data - so multiple interpolators could be used on same data.
 *
 * @author Laurie Nevay
 */

class BDSInterpolator1D
{
public:
  explicit BDSInterpolator1D(BDSArray1DCoords* arrayIn);
  virtual ~BDSInterpolator1D(){;}

  /// Public interface to a 1D interpolator. Returns Geant4 type as that's what will be needed.
  G4ThreeVector GetInterpolatedValue(G4double x) const;

  /// Accessor for the underlying array data.
  inline const BDSArray1DCoords* Array() const {return array;}

  /// Accessor for the active dimension this represents.
  inline BDSDimensionType FirstDimension() const {return array->FirstDimension();}

protected:
  /// Each derived class should implement this function. Note T suffix (was templated)
  /// to distinguish it from GetInterpolatedValue which returns Geant4 types and is
  /// the main interface to should have the clean name.
  virtual BDSFieldValue GetInterpolatedValueT(G4double x) const = 0;
  
  /// The field data.
  BDSArray1DCoords* array;
};

#endif
