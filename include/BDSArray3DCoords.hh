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
#ifndef BDSARRAY3DCOORDS_H
#define BDSARRAY3DCOORDS_H

#include "BDSArray4DCoords.hh"
#include "BDSDimensionType.hh"

#include "globals.hh"

#include <ostream>

/**
 * @brief 3D array with spatial mapping derived from BDSArray4DCoords.
 *
 * Internally dimensions are x,y,z but these can represent different
 * coordinates. Hence the possibility of naming with BDSDimensionType.
 *
 * @author Laurie Nevay
 */

class BDSArray3DCoords: public BDSArray4DCoords
{
public:
  BDSArray3DCoords(G4int nX, G4int nY, G4int nZ,
		   G4double xMinIn, G4double xMaxIn,
		   G4double yMinIn, G4double yMaxIn,
		   G4double zMinIn, G4double zMaxIn,
		   BDSDimensionType xDimensionIn = BDSDimensionType::x,
		   BDSDimensionType yDimensionIn = BDSDimensionType::y,
		   BDSDimensionType zDimensionIn = BDSDimensionType::z);
  virtual ~BDSArray3DCoords(){;}

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray3DCoords const &a);

  /// Accessor for dimension that the data represents (first).
  inline BDSDimensionType FirstDimension() const {return xDimension;}

  /// Accessor for dimension that the data represents (second).
  inline BDSDimensionType SecondDimension() const {return yDimension;}

  /// Accessor for dimension that the data represents (second).
  inline BDSDimensionType ThirdDimension() const {return zDimension;}

private:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray3DCoords() = delete;

  /// Which dimension the contained data represents spatially. Always referred to
  /// locally as 'x' but may represent another dimension.
  BDSDimensionType xDimension;
  BDSDimensionType yDimension;
  BDSDimensionType zDimension;
};

#endif
