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
#include "BDSOutputROOTEventAperture.hh"

#ifndef __ROOTBUILD__
#include "CLHEP/Units/SystemOfUnits.h"
#include "BDSHitEnergyDeposition.hh"
#include "BDSTrajectoryPoint.hh"
#endif

ClassImp(BDSOutputROOTEventAperture)

BDSOutputROOTEventAperture::BDSOutputROOTEventAperture()
{
  FlushLocal();
}

BDSOutputROOTEventAperture::~BDSOutputROOTEventAperture()
{;}

void BDSOutputROOTEventAperture::Fill(const BDSOutputROOTEventAperture* other)
{
  if (!other)
    {return;}
  n        = other->n;
  energy   = other->energy;
  S        = other->S;
  weight   = other->weight;
  isPrimary = other->isPrimary;
  partID   = other->partID;
  turn     = other->turn;
  x        = other->x;
  y        = other->y;
  xp       = other->xp;
  yp       = other->yp;
  T        = other->T;  
  kineticEnergy = other->kineticEnergy;
  isIon    = other->isIon;
  ionA     = other->ionA;
  ionZ     = other->ionZ;
  trackID  = other->trackID;
  parentID = other->parentID;
  modelID  = other->modelID;
}

void BDSOutputROOTEventAperture::FlushLocal()
{
  n = 0;
  energy.clear();
  S.clear();
  weight.clear();
  isPrimary.clear();
  partID.clear();
  turn.clear();
  x.clear();
  y.clear();
  xp.clear();
  yp.clear();
  T.clear();
  kineticEnergy.clear();
  isIon.clear();
  ionA.clear();
  ionZ.clear();
  trackID.clear();
  parentID.clear();
  modelID.clear();
}
