/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#ifndef BDSINTERPOLATE1DLINEARMAG_H
#define BDSINTERPOLATE1DLINEARMAG_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator1D.hh"

#include "G4Types.hh"

class BDSArray1DCoords;

/** 
 * @brief Linear interpolation over 1d array including magnitude interpolation.
 * 
 * Does not own array - so multiple interpolators could be used on same data.
 * 
 * @author Laurie Nevay
 */

class BDSInterpolator1DLinearMag: public BDSInterpolator1D
{
public:
  explicit BDSInterpolator1DLinearMag(BDSArray1DCoords* arrayIn);
  virtual ~BDSInterpolator1DLinearMag();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x) const;

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator1DLinearMag() = delete;
};

#endif
