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
#include "BDSOutputROOTEventLoss.hh"

#ifndef __ROOTBUILD__
#include "CLHEP/Units/SystemOfUnits.h"
#include "BDSHitEnergyDeposition.hh"
#include "BDSTrajectoryPoint.hh"
#endif

ClassImp(BDSOutputROOTEventLoss)

BDSOutputROOTEventLoss::BDSOutputROOTEventLoss():
  storeTurn(false),
  storeLinks(false),
  storeModelID(false),
  storeLocal(false),
  storeGlobal(false),
  storeTime(false),
  storeStepLength(false)
{
  Flush();
}

BDSOutputROOTEventLoss::BDSOutputROOTEventLoss(bool storeTurnIn,
					       bool storeLinksIn,
					       bool storeModelIDIn,
					       bool storeLocalIn,
					       bool storeGlobalIn,
					       bool storeTimeIn,
					       bool storeStepLengthIn,
					       bool storePreStepKineticEnergyIn):
  storeTurn(storeTurnIn),
  storeLinks(storeLinksIn),
  storeModelID(storeModelIDIn),
  storeLocal(storeLocalIn),
  storeGlobal(storeGlobalIn),
  storeTime(storeTimeIn),
  storeStepLength(storeStepLengthIn),
  storePreStepKineticEnergy(storePreStepKineticEnergyIn)
{
  Flush();
}

BDSOutputROOTEventLoss::~BDSOutputROOTEventLoss()
{;}

#ifndef __ROOTBUILD__

void BDSOutputROOTEventLoss::Fill(const BDSTrajectoryPoint* hit)
{
  n++;
  energy.push_back( (float &&) hit->GetEnergy() / CLHEP::GeV);
  S.push_back     ( (float &&) hit->GetPostS()  / CLHEP::m);
  weight.push_back( (float &&) hit->GetPostWeight());
  modelID.push_back((int &&)   hit->GetBeamLineIndex());

  if (storeTurn)
    {turn.push_back((int &&)   hit->GetTurnsTaken());}

  if (storeLocal)
    {
      const G4ThreeVector& pos = hit->GetPostPosLocal();
      x.push_back( (float &&) pos.x() / CLHEP::m);
      y.push_back( (float &&) pos.y() / CLHEP::m);
      z.push_back( (float &&) pos.z() / CLHEP::m);
    }
  if (storeGlobal)
    {
      const G4ThreeVector& pos = hit->GetPosition();
      X.push_back( (float &&) pos.x() / CLHEP::m);
      Y.push_back( (float &&) pos.y() / CLHEP::m);
      Z.push_back( (float &&) pos.z() / CLHEP::m);
    }
  if (storeTime)
    {
      T.push_back( (float &&) hit->GetPostGlobalTime() / CLHEP::ns);
    }

  // don't store stepLength for trajectory point - not possible
  // don't store kinetic energy for trajectory point - not possible
}
void BDSOutputROOTEventLoss::Fill(const BDSHitEnergyDeposition* hit)
{
  n++;
  energy.push_back( (float &&) (hit->GetEnergy() / CLHEP::GeV));
  S.push_back     ( (float &&) (hit->GetSHit()   / CLHEP::m));
  weight.push_back( (float &&)  hit->GetWeight());

  if (storeTurn)
    {turn.push_back( hit->GetTurnsTaken());}

  if (storeLinks)
    {
      partID.push_back(hit->GetPartID());
      trackID.push_back(hit->GetTrackID());
      parentID.push_back(hit->GetParentID());   
    }
  
  if (storeModelID)
    {modelID.push_back(hit->GetBeamlineIndex());}
  
  if (storeLocal)
    {
      x.push_back( (float &&) (hit->Getx() / CLHEP::m));
      y.push_back( (float &&) (hit->Gety() / CLHEP::m));
      z.push_back( (float &&) (hit->Getz() / CLHEP::m));
    }
  
  if(storeGlobal)
    {
      X.push_back( (float &&) (hit->GetX() / CLHEP::m));
      Y.push_back( (float &&) (hit->GetY() / CLHEP::m));
      Z.push_back( (float &&) (hit->GetZ() / CLHEP::m));
    }

  if (storeTime)
    {T.push_back( (float &&) hit->GetGlobalTime() / CLHEP::ns);}

  if (storeStepLength)
    {stepLength.push_back( (float &&) hit->GetStepLength() / CLHEP::m);}
  
  if (storePreStepKineticEnergy)
    {preStepKineticEnergy.push_back( (float &&) hit->GetPreStepKineticEnergy() / CLHEP::GeV);}
}

#endif

void BDSOutputROOTEventLoss::Fill(const BDSOutputROOTEventLoss* other)
{
  if (!other)
    {return;}
  n        = other->n;
  energy   = other->energy;
  S        = other->S;
  weight   = other->weight;
  partID   = other->partID;
  trackID  = other->trackID;
  parentID = other->parentID;
  modelID  = other->modelID;
  turn     = other->turn;
  x = other->x;
  y = other->y;
  z = other->z;
  X = other->X;
  Y = other->Y;
  Z = other->Z;
  T = other->T;  
  stepLength           = other->stepLength;
  preStepKineticEnergy = other->preStepKineticEnergy;
}

void BDSOutputROOTEventLoss::Flush()
{
  n = 0;
  energy.clear();
  S.clear();
  weight.clear();
  partID.clear();
  trackID.clear();
  parentID.clear();
  modelID.clear();
  turn.clear();
  x.clear();
  y.clear();
  z.clear();
  X.clear();
  Y.clear();
  Z.clear();
  T.clear();
  stepLength.clear();
  preStepKineticEnergy.clear();
}
