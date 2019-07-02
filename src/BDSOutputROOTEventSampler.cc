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
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTGeant4Data.hh"

#include "TTree.h"

class BDSOutputROOTGeant4Data;

#ifndef __ROOTBUILD__
#include "BDSHitSampler.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSPhysicalConstants.hh"
#include "BDSPrimaryVertexInformationV.hh"

#include "globals.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include <cmath>
#endif

templateClassImp(BDSOutputROOTEventSampler)

template <class U>
BDSOutputROOTGeant4Data* BDSOutputROOTEventSampler<U>::particleTable = nullptr;

template <class U>
BDSOutputROOTEventSampler<U>::BDSOutputROOTEventSampler():
  samplerName("sampler")
{
  Flush();
}

template <class U>
BDSOutputROOTEventSampler<U>::BDSOutputROOTEventSampler(std::string samplerNameIn):
  samplerName(samplerNameIn)
{
  Flush();
}

template
<class U> BDSOutputROOTEventSampler<U>::~BDSOutputROOTEventSampler()
{;}

#ifndef __ROOTBUILD__
template <class U>
void BDSOutputROOTEventSampler<U>::Fill(const BDSHitSampler* hit,
					G4bool storeMass,
					G4bool storeCharge,
					G4bool storePolarCoords,
					G4bool storeElectrons,
					G4bool storeRigidity,
					G4bool storeKineticEnergy)
{
  // get single values
  n++;
  z = (U) (hit->coords.z / CLHEP::m);
  S = (U) (hit->coords.s / CLHEP::m);
  
  energy.push_back((U &&) (hit->coords.totalEnergy / CLHEP::GeV));
  x.push_back((U &&) (hit->coords.x / CLHEP::m));
  y.push_back((U &&) (hit->coords.y / CLHEP::m));

  xp.push_back((U &&) (hit->coords.xp / CLHEP::radian));
  yp.push_back((U &&) (hit->coords.yp / CLHEP::radian));
  zp.push_back((U &&) (hit->coords.zp / CLHEP::radian));
  T.push_back((U &&) (hit->coords.T / CLHEP::ns));
  
  modelID = hit->beamlineIndex;
  
  weight.push_back((U) (hit->coords.weight));
  partID.push_back(hit->pdgID);
  parentID.push_back(hit->parentID);
  trackID.push_back(hit->trackID);
  turnNumber.push_back(hit->turnsTaken);

  // require mass to calculate kinetic energ
  if (storeMass)
    {mass.push_back((double)(hit->mass / CLHEP::GeV));}

  if (storeCharge)
    {charge.push_back((int)(hit->charge / (G4double)CLHEP::eplus));}

  if (storeKineticEnergy)
    {kineticEnergy.push_back((double)(hit->coords.totalEnergy - hit->mass) / CLHEP::GeV);}

  if (storeRigidity)
    {rigidity.push_back((double)hit->rigidity/(CLHEP::tesla*CLHEP::m));}

  if (storePolarCoords)
    {FillPolarCoords(hit->coords);}

  if (storeElectrons)
    {nElectrons.push_back((int)hit->nElectrons);}
}

template <class U>
void BDSOutputROOTEventSampler<U>::Fill(const BDSParticleCoordsFull& coords,
					const G4double chargeIn,
					const G4int    pdgID,
					const G4int    turnsTaken,
					const G4int    beamlineIndex,
					const G4int    nElectronsIn,
					const G4double massIn,
					const G4double rigidityIn,
					G4bool fillIon)
{
  trackID.push_back(n); // we assume multiple primaries are linearly increasing in track number
  n++;
  energy.push_back((U &&) (coords.totalEnergy / CLHEP::GeV));  
  x.push_back((U &&)  (coords.x  / CLHEP::m));
  y.push_back((U &&)  (coords.y  / CLHEP::m));
  z = (U) (coords.z / CLHEP::m);
  xp.push_back((U &&) (coords.xp / CLHEP::radian));
  yp.push_back((U &&) (coords.yp / CLHEP::radian));
  zp.push_back((U &&) (coords.zp / CLHEP::radian));
  T.push_back((U &&) (coords.T / CLHEP::ns));
  weight.push_back((const U &) coords.weight);
  partID.push_back(pdgID);
  parentID.push_back(0);
  modelID = beamlineIndex;
  turnNumber.push_back(turnsTaken);
  S = (U) (coords.s / CLHEP::GeV);

  // always store optional bits for primary
  charge.push_back((int)(chargeIn / (G4double)CLHEP::eplus));
  mass.push_back((double)(massIn / CLHEP::GeV));
  rigidity.push_back((double)rigidityIn /(CLHEP::tesla*CLHEP::m));
  nElectrons.push_back((int)nElectronsIn);
  kineticEnergy.push_back((double)(coords.totalEnergy - massIn) / CLHEP::GeV);
  FillPolarCoords(coords);
  if (fillIon)
    {FillIon();}
}

template <class U>
void BDSOutputROOTEventSampler<U>::FillPolarCoords(const BDSParticleCoordsFull& coords)
{
  double xCoord  = coords.x  / CLHEP::m;
  double yCoord  = coords.y  / CLHEP::m;
  double xpCoord = coords.xp / CLHEP::radian;
  double ypCoord = coords.yp / CLHEP::radian;

  // we have to tolerate possible sqrt errors here
  double rValue = std::sqrt(std::pow(xCoord, 2) + std::pow(yCoord, 2));
  if (!std::isnormal(rValue))
    {rValue = 0;}
  r.push_back(static_cast<U>(rValue));
  
  double rpValue = std::sqrt(std::pow(xpCoord, 2) + std::pow(ypCoord, 2));
  if (!std::isnormal(rpValue))
    {rpValue = 0;}
  rp.push_back(static_cast<U>(rpValue));

  double phiValue = std::atan2(xCoord, yCoord);
  if (!std::isnormal(phiValue))
    {phiValue = -1;}
  phi.push_back(static_cast<U>(phiValue));

  double phipValue = std::atan2(xpCoord, ypCoord);
  if (!std::isnormal(phipValue))
    {phipValue = -1;}
  phip.push_back(static_cast<U>(phipValue));
}

template <class U>
void BDSOutputROOTEventSampler<U>::Fill(const BDSPrimaryVertexInformationV* vertexInfos,
					const G4int turnsTaken)
{
  for (const auto& vertexInfo : vertexInfos->vertices)
    {
      Fill(vertexInfo.primaryVertex.local,
	   vertexInfo.charge,
	   vertexInfo.pdgID,
	   turnsTaken,
	   vertexInfo.primaryVertex.beamlineIndex,
	   vertexInfo.nElectrons,
	   vertexInfo.mass,
	   vertexInfo.rigidity,
	   false);
    }
  FillIon();
}

//#else
//void BDSOutputROOTEventSampler::SetBranchAddress(TTree *)
//{}
#endif

template <class U>
void BDSOutputROOTEventSampler<U>::Fill(const BDSOutputROOTEventSampler<U>* other)
{
  if (!other)
    {return;}
  n      = other->n;
  energy = other->energy;
  x      = other->x;
  y      = other->y;
  z      = other->z;
  xp     = other->xp;
  yp     = other->yp;
  zp     = other->zp;
  T      = other->T;

  weight     = other->weight;
  partID     = other->partID;
  parentID   = other->parentID;
  trackID    = other->trackID;
  modelID    = other->modelID;
  turnNumber = other->turnNumber;
  S          = other->S;

  r          = other->r;
  rp         = other->rp;
  phi        = other->phi;
  phip       = other->phip;

  charge        = other->charge;
  kineticEnergy = other->kineticEnergy;
  mass          = other->mass;
  rigidity      = other->rigidity;

  isIon = other->isIon;
  ionA  = other->ionA;
  ionZ  = other->ionZ;
  nElectrons = other->nElectrons;
}

template <class U> void BDSOutputROOTEventSampler<U>::SetBranchAddress(TTree *)
{;}

template <class U> void BDSOutputROOTEventSampler<U>::Flush()
{
  n = 0;
  energy.clear();
  x.clear();
  y.clear();
  z = 0.0;
  xp.clear();
  yp.clear();
  zp.clear();
  T.clear();
  weight.clear();
  partID.clear();
  parentID.clear();
  trackID.clear();
  modelID = -1;
  turnNumber.clear();

  S = 0.0;

  r.clear();
  rp.clear();
  phi.clear();
  phip.clear();

  charge.clear();
  kineticEnergy.clear();
  mass.clear();
  rigidity.clear();
  isIon.clear();
  ionA.clear();
  ionZ.clear();
  nElectrons.clear();
}

template <class U>
std::vector<U> BDSOutputROOTEventSampler<U>::getKineticEnergy()
{
  std::vector<U> result((unsigned long)n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {result[i] = (U)particleTable->KineticEnergy(partID[i], energy[i]);}
  return result;
}

template <class U>
std::vector<U> BDSOutputROOTEventSampler<U>::getMass()
{
  std::vector<U> result((unsigned long)n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {result[i] = (U)particleTable->Mass(partID[i]);}
  return result;
}

template <class U>
std::vector<U> BDSOutputROOTEventSampler<U>::getRigidity()
{
  std::vector<U> result((unsigned long)n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {result[i] = (U)particleTable->Rigidity(partID[i], energy[i]);}
  return result;
}

template <class U>
std::vector<bool> BDSOutputROOTEventSampler<U>::getIsIon()
{
  bool useNElectrons = nElectrons.size() > 0;
  std::vector<bool> result((unsigned long)n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {
      result[i] = particleTable->IsIon(partID[i]);
      if (useNElectrons)
        {result[i] = result[i] || nElectrons[i] > 0;}
    }
  return result;
}

template <class U>
std::vector<int> BDSOutputROOTEventSampler<U>::getIonA()
{
  std::vector<int> result((unsigned long)n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {result[i] = particleTable->IonA(partID[i]);}
  return result;
}

template <class U>
std::vector<int> BDSOutputROOTEventSampler<U>::getIonZ()
{
  std::vector<int> result((unsigned long)n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {result[i] = particleTable->IonZ(partID[i]);}
  return result;
}

template class BDSOutputROOTEventSampler<float>;
template class BDSOutputROOTEventSampler<double>;
