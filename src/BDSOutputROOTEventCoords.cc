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
#include "BDSOutputROOTEventCoords.hh"

#ifndef __ROOTBUILD__
#include "BDSParticleCoords.hh"
#endif

ClassImp(BDSOutputROOTEventCoords)

BDSOutputROOTEventCoords::BDSOutputROOTEventCoords():
  x(0),
  y(0),
  z(0),
  xp(0),
  yp(0),
  zp(0),
  T(0)
{;}

BDSOutputROOTEventCoords::~BDSOutputROOTEventCoords()
{;}

void BDSOutputROOTEventCoords::Flush()
{
  x  = 0;
  y  = 0;
  z  = 0;
  xp = 0;
  yp = 0;
  zp = 0;
  T  = 0;
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventCoords::Fill(const BDSParticleCoords& coords)
{
  x  = coords.x  / CLHEP::m;
  y  = coords.y  / CLHEP::m;
  z  = coords.z  / CLHEP::m;
  xp = coords.xp / CLHEP::rad;
  yp = coords.yp / CLHEP::rad;
  zp = coords.zp / CLHEP::rad;
  T  = coords.T  / CLHEP::ns; // to match sampler data
}
#endif

void BDSOutputROOTEventCoords::Fill(const BDSOutputROOTEventCoords* other)
{
  if (!other)
    {return;}
  
  x  = other->x;
  y  = other->y;
  z  = other->z;
  xp = other->xp;
  yp = other->yp;
  zp = other->zp;
  T  = other->T;
}
