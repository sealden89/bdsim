/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSInterpolator3DCubic.hh"
#include "BDSInterpolatorRoutines.hh"

#include "globals.hh"

#include <cmath>

BDSInterpolator3DCubic::BDSInterpolator3DCubic(BDSArray3DCoords* arrayIn):
  BDSInterpolator3D(arrayIn)
{;}

BDSInterpolator3DCubic::~BDSInterpolator3DCubic()
{;}

BDSFieldValue BDSInterpolator3DCubic::GetInterpolatedValueT(G4double x,
							    G4double y,
							    G4double z) const
{
  G4double xarr = array->ArrayCoordsFromX(x);
  G4double yarr = array->ArrayCoordsFromY(y);
  G4double zarr = array->ArrayCoordsFromZ(z);
  
  G4double x1 = std::floor(xarr);
  G4double y1 = std::floor(yarr);
  G4double z1 = std::floor(zarr);

  BDSFieldValue localData[4][4][4];

  G4double x0 = x1-1;
  G4double y0 = y1-1;
  G4double z0 = z1-1;
  for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
	{
	  for (int k = 0; k < 4; k++)
	    {localData[i][j][k] = array->GetConst(x0+i, y0+j, z0+k);}
	}
    }

  return BDS::Cubic3D(localData, xarr-x1, yarr-y1, zarr-z1);
}
