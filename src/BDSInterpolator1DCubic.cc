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
#include "BDSArray1DCoords.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator1DCubic.hh"
#include "BDSInterpolatorRoutines.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator1DCubic::BDSInterpolator1DCubic(BDSArray1DCoords* arrayIn):
  BDSInterpolator1D(arrayIn)
{;}

BDSInterpolator1DCubic::~BDSInterpolator1DCubic()
{;}

BDSFieldValue BDSInterpolator1DCubic::GetInterpolatedValueT(G4double x) const
{
  G4double xarr = array->ArrayCoordsFromX(x);
  G4double x1   = floor(xarr);

  BDSFieldValue localData[4];

  G4double x0 = x1-1;
  for (int i = 0; i < 4; i++)
    {localData[i] = array->GetConst(x0+i);}

  return BDS::Cubic1D(localData, xarr-x1);
}
