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
#include "BDSInterpolator4D.hh"

#include "globals.hh"

BDSInterpolator4D::BDSInterpolator4D(BDSArray4DCoords* arrayIn):
  array(arrayIn)
{
  if (!array)
    {G4cerr << "Invalid array to construct interpolator on" << G4endl; exit(1);}
}

G4ThreeVector BDSInterpolator4D::GetInterpolatedValue(G4double x,
						      G4double y,
						      G4double z,
						      G4double t) const
{
  BDSFieldValue r = GetInterpolatedValueT(x,y,z,t);
  return G4ThreeVector(r.x(), r.y(), r.z());
}
