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
#ifndef BDSFIELDMAGSEXTUPOLE_H
#define BDSFIELDMAGSEXTUPOLE_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic strength in a sextupole.
 * 
 * The magnetic field is calculated from the sextupole strength parameter
 * "k2" and a design rigidity (brho).
 */

class BDSFieldMagSextupole: public BDSFieldMag
{
public:
  BDSFieldMagSextupole(BDSMagnetStrength const* strength,
		       G4double          const  brho);
  
  virtual ~BDSFieldMagSextupole(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const G4double       t = 0) const;
  
private:
  /// Private default constructor to avoid usage.
  BDSFieldMagSextupole();
  
  /// B'' - second derivative of the magnetic field.
  G4double bDoublePrime;
  
  /// bDoublePrime / 2! cached
  G4double halfBDoublePrime;
};

#endif
