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
#include "BDSArray2DCoordsExtendX.hh"
#include "BDSFieldValue.hh"

#include "globals.hh"

#include <algorithm>
#include <cmath>
#include <ostream>


BDSArray2DCoordsExtendX::BDSArray2DCoordsExtendX(BDSArray2DCoords* arrayIn):
  BDSArray2DCoords(*arrayIn),
  returnValue(BDSFieldValue())
{;}

G4bool BDSArray2DCoordsExtendX::OutsideCoords(const G4double x,
					      const G4double y,
					      const G4double z,
					      const G4double t) const
{
  G4bool rx = x <  xMin || x > xMax;
  G4bool ry = y < -yMax || y > yMax;
  G4bool rz = z <  zMin || z > zMax;
  G4bool rt = t <  tMin || t > tMax;
  return rx || ry || rz || rt;
}

G4double BDSArray2DCoordsExtendX::ArrayCoordsFromY(const G4double y) const
{
  return (y + yMax) / yStep;
}

G4int BDSArray2DCoordsExtendX::NearestY(const G4double y) const
{
  return (G4int)round((y+yMax)/yStep);
}

const BDSFieldValue& BDSArray2DCoordsExtendX::GetConst(const G4int x,
						       const G4int y,
						       const G4int z,
						       const G4int t) const
{
  if (Outside(x,y,z,t))
    {return defaultValue;}

  // Data is really
  // A

  // This interface makes it look like (0,0) bottom left, top right (xsize, 2*ysize)
  // A
  // -
  // B

  G4int yi;
  if (y < nY)
    {// B half - coordinate flip is a subtraction in array coords
      yi = nY - 1 - y;
    }
  else
    {// A half
      yi = y + nY; // just offset index
    }
  
  returnValue = BDSArray2DCoords::GetConst(x,yi,z,t);

  return returnValue;
}

G4bool BDSArray2DCoordsExtendX::Outside(const G4int x,
					const G4int y,
					const G4int z,
					const G4int t) const
{
  G4bool rx = x < 0 || x > nX-1;
  G4bool ry = y < 0 || y > 2*(nY-1);
  G4bool rz = z < 0 || z > nZ-1;
  G4bool rt = t < 0 || t > nT-1;
  return rx || ry || rz || rt;
}

std::ostream& BDSArray2DCoordsExtendX::Print(std::ostream& out) const
{
  std::ostream& out2 = BDSArray2DCoords::Print(out);

  out2 << G4endl << "REFLECTED VERSION" << G4endl;
  
  for (G4int t = 0; t < nT; t++)
    {
      for (G4int z = 0; z < nZ; z++)
	{
	  for (G4int y = 0; y < 2*nY; y++)
	    {
	      for (G4int x = 0; x < nX; x++)
		{out2 << GetConst(x,y,z,t) << G4endl;}
	    }
	}
    }
  return out2;
}

std::ostream& operator<< (std::ostream& out, BDSArray2DCoordsExtendX const &a)
{
  return a.Print(out);
}
