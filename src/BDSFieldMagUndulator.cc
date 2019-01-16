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
#include "BDSDebug.hh"
#include "BDSFieldMagUndulator.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

BDSFieldMagUndulator::BDSFieldMagUndulator(BDSMagnetStrength const* strength)
{
  wavenumber = (CLHEP::twopi)/(*strength)["length"];
  B = (*strength)["field"] / CLHEP::tesla;
  finiteStrength = BDS::IsFinite(B);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B = " << B << G4endl;
#endif
}

G4ThreeVector BDSFieldMagUndulator::GetField(const G4ThreeVector& position,
					     const G4double       /*t*/) const
{

  G4ThreeVector field;

  field[0] = 0;
  field[1] = B * std::cos(position.z() * wavenumber) * std::cosh(position.y() * wavenumber);
  field[2] = -B * std::sin(position.z() * wavenumber) * std::sinh(position.y() * wavenumber);

  return field;
}
