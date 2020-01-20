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
#ifndef BDSFIELDEMGLOBAL_H
#define BDSFIELDEMGLOBAL_H

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "BDSAuxiliaryNavigator.hh"
#include "BDSFieldEM.hh"

#include <utility>

/**
 * @brief A base class for electro-magnetic fields in local to be used in global coordinates.
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

class BDSFieldEMGlobal: public BDSFieldEM, public BDSAuxiliaryNavigator
{
public:
  explicit BDSFieldEMGlobal(BDSFieldEM* fieldIn);
  virtual ~BDSFieldEMGlobal();

  /// Get the field - local coordinates. First G4ThreeVector is B field, second is E Field
  /// x,y,z respectively. Apply the global to local transform, query the wrapped field object
  /// and transform this field to global coordinates before returning.
  virtual std::pair<G4ThreeVector,G4ThreeVector> GetField(const G4ThreeVector& position,
                                                          const G4double       t) const;

  /// Necessary overload for Geant4
  virtual G4bool DoesFieldChangeEnergy() const {return true;}
  
private:
  /// Private default constructor to force use of supplied constructor
  BDSFieldEMGlobal();

  /// The field on which this is based.
  BDSFieldEM* field;
};

#endif
