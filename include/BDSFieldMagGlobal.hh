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
#ifndef BDSFIELDMAGGLOBAL_H
#define BDSFIELDMAGGLOBAL_H

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "BDSAuxiliaryNavigator.hh"
#include "BDSFieldMag.hh"

/**
 * @brief A base class for magnetic fields in local to be used in global coordinates.
 * 
 * This base class provides the aggregative inheritance and utility functions
 * for magnetic fields in local coordinates to be used in global coordinates.
 * 
 * Constness is particularly important here as member functions are called
 * from inside G4MagneticField::GetField function which is const.
 *
 * This owns the field it wraps.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMagGlobal: public BDSFieldMag, public BDSAuxiliaryNavigator
{
public:
  explicit BDSFieldMagGlobal(BDSFieldMag* fieldIn);
  virtual ~BDSFieldMagGlobal();

  /// As we use a discrete member field object, we do not need to apply the
  /// transform. Override default method and just directly call GetField().
  virtual G4ThreeVector GetFieldTransformed(const G4ThreeVector& position,
                                            const G4double       t) const;

  /// Apply the global to local transform, query the wrapped field object
  /// and transform this field to global coordinates before returning.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const G4double       t = 0) const;
  
private:
  /// Private default constructor to force use of supplied constructor
  BDSFieldMagGlobal();

  /// The field on which this is based.
  BDSFieldMag* field;
};

#endif
