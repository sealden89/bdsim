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
#include "BDSOutputROOTGeant4Data.hh"

#ifndef __ROOTBUILD__
#include "CLHEP/Units/SystemOfUnits.h"
#include "BDSHitApertureImpact.hh"
#endif

ClassImp(BDSOutputROOTEventAperture)

BDSOutputROOTGeant4Data* BDSOutputROOTEventAperture::particleTable = nullptr;

BDSOutputROOTEventAperture::BDSOutputROOTEventAperture()
{
  FlushLocal();
}

BDSOutputROOTEventAperture::~BDSOutputROOTEventAperture()
{;}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventAperture::Fill(const BDSHitApertureImpact* hit,
				      G4bool isPrimaryFirstImpact)
{
  n++;
  energy.push_back((float)hit->totalEnergy / CLHEP::GeV);
  S.push_back(hit->S / CLHEP::m);
  weight.push_back(hit->weight);
  isPrimary.push_back(hit->parentID == 0);
  firstPrimaryImpact.push_back(isPrimaryFirstImpact);
  partID.push_back(hit->partID);
  turn.push_back(hit->turnsTaken);
  x.push_back(hit->x / CLHEP::m);
  y.push_back(hit->y / CLHEP::m);
  xp.push_back(hit->xp / CLHEP::rad);
  yp.push_back(hit->yp / CLHEP::rad);
  T.push_back(hit->globalTime / CLHEP::ns);
  kineticEnergy.push_back((float)hit->preStepKineticEnergy / CLHEP::GeV);
  G4int pid = hit->partID;
  
  G4bool ii = false;
  if (particleTable)
    {ii = particleTable->IsIon(pid);}
  if (particleTable && ii) // avoid nested ifs with duplicated setting of variables by doing it this way
    {
      auto& ionInfo = particleTable->GetIonInfo(pid);
      isIon.push_back(true);
      ionA.push_back(ionInfo.a);
      ionZ.push_back(ionInfo.z);
    }
  else
    {
      isIon.push_back(false);
      ionA.push_back(0);
      ionZ.push_back(0);
    }      
  
  trackID.push_back(hit->trackID);
  parentID.push_back(hit->parentID);
  modelID.push_back(hit->beamlineIndex);
}

#endif

void BDSOutputROOTEventAperture::Fill(const BDSOutputROOTEventAperture* other)
{
  if (!other)
    {return;}
  n        = other->n;
  energy   = other->energy;
  S        = other->S;
  weight   = other->weight;
  isPrimary = other->isPrimary;
  firstPrimaryImpact = other->firstPrimaryImpact;
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
  firstPrimaryImpact.clear();
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
