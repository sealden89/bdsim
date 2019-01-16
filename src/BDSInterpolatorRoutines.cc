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
#include "BDSFieldValue.hh"
#include "BDSInterpolatorRoutines.hh"

#include "G4Types.hh"


BDSFieldValue BDS::Linear1D(const BDSFieldValue p[2],
			    const G4double      x)
{
  return p[0]*(1.-x) + p[1]*x;
}

BDSFieldValue BDS::Linear2D(const BDSFieldValue p[2][2],
			    const G4double      x,
			    const G4double      y)
{
  BDSFieldValue arr[2];
  arr[0] = BDS::Linear1D(p[0], y);
  arr[1] = BDS::Linear1D(p[1], y);
  return BDS::Linear1D(arr, x);
}

BDSFieldValue BDS::Linear3D(const BDSFieldValue p[2][2][2],
			    const G4double      x,
			    const G4double      y,
			    const G4double      z)
{
  BDSFieldValue arr[2];
  arr[0] = BDS::Linear2D(p[0], y, z);
  arr[1] = BDS::Linear2D(p[1], y, z);
  return BDS::Linear1D(arr, x);
}

BDSFieldValue BDS::Linear4D(const BDSFieldValue p[2][2][2][2],
			    const G4double      x,
			    const G4double      y,
			    const G4double      z,
			    const G4double      t)
{
  BDSFieldValue arr[2];
  arr[0] = BDS::Linear3D(p[0], y, z, t);
  arr[1] = BDS::Linear3D(p[1], y, z, t);
  return BDS::Linear1D(arr, x);
}

BDSFieldValue BDS::Cubic1D(const BDSFieldValue p[4],
			   const G4double      x)
{
  return p[1]+0.5 * x*(p[2]-p[0]+x*(2.*p[0]-5.*p[1]+4.*p[2]-p[3]+x*(3.*(p[1]-p[2])+p[3]-p[0])));
}

BDSFieldValue BDS::Cubic2D(const BDSFieldValue p[4][4],
			   const G4double      x,
			   const G4double      y)
{
  BDSFieldValue arr[4];
  arr[0] = BDS::Cubic1D(p[0], y);
  arr[1] = BDS::Cubic1D(p[1], y);
  arr[2] = BDS::Cubic1D(p[2], y);
  arr[3] = BDS::Cubic1D(p[3], y);
  return BDS::Cubic1D(arr, x);
}

BDSFieldValue BDS::Cubic3D(const BDSFieldValue p[4][4][4],
			   const G4double      x,
			   const G4double      y,
			   const G4double      z)
{
  BDSFieldValue arr[4];
  arr[0] = BDS::Cubic2D(p[0], y, z);
  arr[1] = BDS::Cubic2D(p[1], y, z);
  arr[2] = BDS::Cubic2D(p[2], y, z);
  arr[3] = BDS::Cubic2D(p[3], y, z);
  return BDS::Cubic1D(arr, x);
}

BDSFieldValue BDS::Cubic4D(const BDSFieldValue p[4][4][4][4],			   
			   const G4double      x,
			   const G4double      y,
			   const G4double      z,
			   const G4double      t)
{
  BDSFieldValue arr[4];
  arr[0] = BDS::Cubic3D(p[0], y, z, t);
  arr[1] = BDS::Cubic3D(p[1], y, z, t);
  arr[2] = BDS::Cubic3D(p[2], y, z, t);
  arr[3] = BDS::Cubic3D(p[3], y, z, t);
  return BDS::Cubic1D(arr, x);
}
