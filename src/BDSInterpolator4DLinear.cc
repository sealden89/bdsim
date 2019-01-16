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
#include "BDSInterpolator4DLinear.hh"
#include "BDSInterpolatorRoutines.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator4DLinear::BDSInterpolator4DLinear(BDSArray4DCoords* arrayIn):
  BDSInterpolator4D(arrayIn)
{;}

BDSInterpolator4DLinear::~BDSInterpolator4DLinear()
{;}

BDSFieldValue BDSInterpolator4DLinear::GetInterpolatedValueT(G4double x,
							     G4double y,
							     G4double z,
							     G4double t) const
{
  G4double xarr = array->ArrayCoordsFromX(x);
  G4double yarr = array->ArrayCoordsFromY(y);
  G4double zarr = array->ArrayCoordsFromZ(z);
  G4double tarr = array->ArrayCoordsFromT(t);
  
  G4double x0 = floor(xarr);
  G4double y0 = floor(yarr);
  G4double z0 = floor(zarr);
  G4double t0 = floor(tarr);

  BDSFieldValue localData[2][2][2][2];

  for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
	{
	  for (int k = 0; k < 2; k++)
	    {
	      for (int l = 0; l < 2; l++)
		{localData[i][j][k][l] = array->GetConst(x0+i, y0+j, z0+k, t0+l);}
	    }
	}
    }

  BDSFieldValue result = BDS::Linear4D(localData, xarr-x0, yarr-y0, zarr-z0, tarr-t0);

  return result;
}
