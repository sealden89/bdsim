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
#ifndef BDSFIELDEZERO_H
#define BDSFIELDEZERO_H

#include "BDSFieldE.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

/**
 * @brief Null E field - for special cases where we need a valid object.
 * 
 * Simply returns 0,0,0,0,0,0 for all field components.
 *
 * @author Laurie Nevay.
 */

class BDSFieldEZero: public BDSFieldE
{
public:
  BDSFieldEZero(){finiteStrength = false;}
  
  virtual ~BDSFieldEZero(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& /*position*/,
				 const G4double       /*t*/ = 0) const
  {return G4ThreeVector(0,0,0);}
};

#endif
