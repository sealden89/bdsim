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
#include "BDSArray3DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator3DLinear.hh"
#include "BDSInterpolatorRoutines.hh"

#include "G4Types.hh"

BDSInterpolator3DLinear::BDSInterpolator3DLinear(BDSArray3DCoords* arrayIn):
  BDSInterpolator3D(arrayIn)
{;}

BDSInterpolator3DLinear::~BDSInterpolator3DLinear()
{;}

BDSFieldValue BDSInterpolator3DLinear::GetInterpolatedValueT(G4double x,
							     G4double y,
							     G4double z) const
{
  BDSFieldValue localData[2][2][2];
  G4double xFrac, yFrac, zFrac;
  array->ExtractSection2x2x2(x, y, z, localData, xFrac, yFrac, zFrac);
  return BDS::Linear3D(localData, xFrac, yFrac, zFrac);
}
