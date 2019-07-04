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
#include "BDSPrimaryVertexInformationV.hh"
#endif

ClassImp(BDSOutputROOTEventCoords)

BDSOutputROOTEventCoords::BDSOutputROOTEventCoords():
  n(0)
{;}

BDSOutputROOTEventCoords::~BDSOutputROOTEventCoords()
{;}

void BDSOutputROOTEventCoords::Flush()
{
  n = 0;
  x.clear();
  y.clear();
  z.clear();
  xp.clear();
  yp.clear();
  zp.clear();
  T.clear();
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventCoords::Fill(const BDSParticleCoords& coords)
{
  n++;
  x.push_back(coords.x  / CLHEP::m);
  y.push_back(coords.y  / CLHEP::m);
  z.push_back(coords.z  / CLHEP::m);
  xp.push_back(coords.xp / CLHEP::rad);
  yp.push_back(coords.yp / CLHEP::rad);
  zp.push_back(coords.zp / CLHEP::rad);
  T.push_back(coords.T  / CLHEP::ns); // to match sampler data
}

void BDSOutputROOTEventCoords::Fill(const BDSPrimaryVertexInformationV* vertexInfos)
{
  for (const auto& vertexInfo : vertexInfos->vertices)
    {Fill(vertexInfo.primaryVertex.global);}
}

#endif

void BDSOutputROOTEventCoords::Fill(const BDSOutputROOTEventCoords* other)
{
  if (!other)
    {return;}

  n  = other->n;
  x  = other->x;
  y  = other->y;
  z  = other->z;
  xp = other->xp;
  yp = other->yp;
  zp = other->zp;
  T  = other->T;
}
