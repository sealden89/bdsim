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
#include "BDSInterpolator4DCubic.hh"
#include "BDSInterpolatorRoutines.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator4DCubic::BDSInterpolator4DCubic(BDSArray4DCoords* arrayIn):
  BDSInterpolator4D(arrayIn)
{;}

BDSInterpolator4DCubic::~BDSInterpolator4DCubic()
{;}

BDSFieldValue BDSInterpolator4DCubic::GetInterpolatedValueT(G4double x,
							    G4double y,
							    G4double z,
							    G4double t) const
{
  G4double xarr = array->ArrayCoordsFromX(x);
  G4double yarr = array->ArrayCoordsFromY(y);
  G4double zarr = array->ArrayCoordsFromZ(z);
  G4double tarr = array->ArrayCoordsFromT(t);
  
  G4double x1 = floor(xarr);
  G4double y1 = floor(yarr);
  G4double z1 = floor(zarr);
  G4double t1 = floor(tarr);

  BDSFieldValue localData[4][4][4][4];

  G4double x0 = x1-1;
  G4double y0 = y1-1;
  G4double z0 = z1-1;
  G4double t0 = t1-1;
  for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
	{
	  for (int k = 0; k < 4; k++)
	    {
	      for (int l = 0; l < 4; l++)
		{localData[i][j][k][l] = array->GetConst(x0+i, y0+j, z0+k, t0+l);}
	    }
	}
    }

  return BDS::Cubic4D(localData, xarr-x1, yarr-y1, zarr-z1, tarr-t1);
}
