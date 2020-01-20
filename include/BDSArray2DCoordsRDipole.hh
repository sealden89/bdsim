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
#ifndef BDSARRAY2DCOORDSRDIPOLE_H
#define BDSARRAY2DCOORDSRDIPOLE_H

#include "BDSArray2DCoords.hh"
#include "BDSFieldValue.hh"

#include "globals.hh"

#include <ostream>

/**
 * @brief A wrapper to achieve 2D reflection of a minimal dipole field solve.
 * 
 * This allows a smaller array representing the positive quadrant in x,y to 
 * be reflected to represent a quadrupole.
 *
 * \verbatim
 *       y
 *       ∧
 *       |
 *    ∧  |  ∧
 *    |  |  | original
 * <---  |  --->
 *       |
 * -------------------> x
 * ∧     |     ∧
 * |     |     |
 * --->  |  <---
 *       |
 *       |
 * \endverbatim
 *
 * This owns the array it wraps.
 *
 * This makes an array look 4 times bigger than it is - ie 2x as big in x and y. The
 * array coordinates are correspondingly shifted. What would've been (0,0) in a 100x100
 * array will now be at index (101,101).
 *
 * This is reflected about 0,0 and can only be so given the implementation.
 *
 * Some interfaces are overloaded and some aren't as are required to be (sizeX for example).
 * The ostream << writes both the raw array and the reflected version too.
 *
 * Note, by return a reference to a non-const member returnValue that may be updated by another
 * query, thread safety could be an issue.
 * 
 * @author Laurie Nevay
 */

class BDSArray2DCoordsRDipole: public BDSArray2DCoords
{
public:
  /// Wrap an existing BDSArray2DCoords instance.
  explicit BDSArray2DCoordsRDipole(BDSArray2DCoords* arrayIn);
  virtual ~BDSArray2DCoordsRDipole(){;}

  /// @{ Overridden from BDSArray4DCoords.
  virtual G4bool OutsideCoords(const G4double x,
			       const G4double y,
			       const G4double z,
			       const G4double t) const;

  virtual G4double ArrayCoordsFromX(const G4double x) const;
  virtual G4double ArrayCoordsFromY(const G4double y) const;

  virtual G4int NearestX(const G4double x) const;
  virtual G4int NearestY(const G4double y) const;
  /// @}

  /// @{ Overridden from BDSArray4D.
  virtual const BDSFieldValue& GetConst(const G4int x,
					const G4int y,
					const G4int z = 0,
					const G4int t = 0) const;

  virtual G4bool Outside(const G4int x,
			 const G4int y,
			 const G4int z,
			 const G4int t) const;
  /// @}

  /// This prints out the raw underlying data, then the reflected version as would normally
  /// be queried.
  virtual std::ostream& Print(std::ostream& out) const;

  /// Delegate function to call polymorphic Print().
  friend std::ostream& operator<< (std::ostream& out, BDSArray2DCoordsRDipole const &a);

private:
  BDSArray2DCoordsRDipole() = delete;

  /// Must return by reference and the true data value may have to be reflected
  /// in one or more dimensions.  Copy to this and return reference to it.
  mutable BDSFieldValue returnValue;
};

#endif
