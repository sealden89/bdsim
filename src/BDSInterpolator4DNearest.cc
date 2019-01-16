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
#include "BDSArray4DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator4DNearest.hh"


BDSInterpolator4DNearest::BDSInterpolator4DNearest(BDSArray4DCoords* arrayIn):
  BDSInterpolator4D(arrayIn)
{;}

BDSInterpolator4DNearest::~BDSInterpolator4DNearest()
{;}

BDSFieldValue BDSInterpolator4DNearest::GetInterpolatedValueT(G4double x,
							      G4double y,
							      G4double z,
							      G4double t) const
{
  G4int xind = array->NearestX(x);
  G4int yind = array->NearestY(y);
  G4int zind = array->NearestZ(z);
  G4int tind = array->NearestT(t);
  // here we're constructing a copy on purpose
  BDSFieldValue result = array->GetConst(xind, yind, zind, tind);
  return result;
}
