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
#ifndef BDSFIELDEGLOBAL_H
#define BDSFIELDEGLOBAL_H

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "BDSAuxiliaryNavigator.hh"
#include "BDSFieldE.hh"

#include <utility>

/**
 * @brief A base class for electric fields in local to be used in global coordinates.
 * 
 * This base class provides the aggregative inheritance and utility functions
 * for magnetic fields in local coordinates to be used in global coordinates.
 * 
 * Constness is particularly important here as member functions are called
 * from inside GetField function which is const.
 *
 * This owns the field it wraps.
 * 
 * @author Laurie Nevay
 */

class BDSFieldEGlobal: public BDSFieldE, public BDSAuxiliaryNavigator
{
public:
  BDSFieldEGlobal() = delete;
  explicit BDSFieldEGlobal(BDSFieldE* fieldIn);
  virtual ~BDSFieldEGlobal();

  /// As we use a discrete member field object, we do not need to apply the
  /// transform. Override default method and just directly call GetField().
  virtual G4ThreeVector GetFieldTransformed(const G4ThreeVector& position,
                                            const G4double       t) const;

  /// Get the field - local coordinates. Apply the global to local transform,
  /// query the wrapped field object and transform this field to global
  /// coordinates before returning.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t) const;

  /// Necessary overload for Geant4
  virtual G4bool DoesFieldChangeEnergy() const {return true;}
  
private:
  /// The field on which this is based.
  BDSFieldE* field;
};

#endif
