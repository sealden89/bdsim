/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#ifndef BDSINTERPOLATE2DLINEARMAG_H
#define BDSINTERPOLATE2DLINEARMAG_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator2D.hh"

#include "G4Types.hh"

class BDSArray2DCoords;

/** 
 * @brief Linear interpolation over 2d array including magnitude interpolation.
 * 
 * Does not own array - so multiple interpolators could be used on same data.
 * 
 * This uses a bilinear algorithm to interpolate a 2d array and return
 * the value at any arbitrary point. If the point lies outside the array
 * the default value for the templated parameter is returned (typically 0).
 * Therefore, the field drops to 0 outside the specified region.
 * 
 * @author Laurie Nevay
 */

class BDSInterpolator2DLinearMag: public BDSInterpolator2D
{
public:
  explicit BDSInterpolator2DLinearMag(BDSArray2DCoords* arrayIn);
  virtual ~BDSInterpolator2DLinearMag();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x, G4double y) const;

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator2DLinearMag() = delete;
};

#endif
