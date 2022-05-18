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
#ifndef BDSFIELDEINTERPOLATED1D_H
#define BDSFIELDEINTERPOLATED1D_H

#include "BDSFieldEInterpolated.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

class BDSInterpolator1D;

/**
 * @brief A 1D field from an interpolated array with any interpolation.
 *
 * Owns interpolator.
 *
 * This provides a simple interface for electric fields to use a 1D
 * interpolator irrespective of which type of interpolator it is.
 *
 * @author Laurie Nevay
 */

class BDSFieldEInterpolated1D: public BDSFieldEInterpolated
{
public:
  BDSFieldEInterpolated1D() = delete;
  explicit BDSFieldEInterpolated1D(BDSInterpolator1D*   interpolatorIn,
				   const G4Transform3D& offset      = G4Transform3D::Identity,
				   G4double             eScalingIn  = 1.0);

  virtual ~BDSFieldEInterpolated1D();

  /// Return the interpolated field value at a given point.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t = 0) const;

  inline const BDSInterpolator1D* Interpolator() const {return interpolator;}

private:
  BDSInterpolator1D* interpolator; ///< Interpolator the field is based on.
  const G4int  dimensionIndex;     ///< Integer index to dimension to use.
  const G4bool time;               ///< Cache of whether to use time coordinate.
};

#endif
