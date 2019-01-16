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
#include "BDSFieldE.hh"
#include "BDSFieldEGlobal.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

BDSFieldEGlobal::BDSFieldEGlobal(BDSFieldE* fieldIn):
  field(fieldIn)
{
  finiteStrength = field->FiniteStrength();
}

BDSFieldEGlobal::~BDSFieldEGlobal()
{
  delete field;
}

G4ThreeVector BDSFieldEGlobal::GetField(const G4ThreeVector& position,
					const G4double       t) const
{
  G4ThreeVector localPosition = ConvertToLocal(position);
  G4ThreeVector localField    = field->GetFieldTransformed(localPosition, t);
  G4ThreeVector globalField   = ConvertAxisToGlobal(localField);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Local Position: " << localPosition << G4endl;
  G4cout << __METHOD_NAME__ << "Local Field:    " << localField    << G4endl;
  G4cout << __METHOD_NAME__ << "Global Field:   " << globalField   << G4endl;
#endif
  return globalField;
}
