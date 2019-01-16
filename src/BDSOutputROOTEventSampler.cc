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
#include "BDSParticleCoordsFull.hh"
#include "BDSSamplerHit.hh"

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
void BDSOutputROOTEventSampler<U>::Fill(const BDSSamplerHit* hit,
					G4bool storeCharge,
					G4bool storePolarCoords)
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

  if (storeCharge)
    {charge.push_back((int)(hit->charge / (G4double)CLHEP::eplus));}

  if (storePolarCoords)
    {FillPolarCoords(hit->coords);}
}

template <class U>
void BDSOutputROOTEventSampler<U>::Fill(const BDSParticleCoordsFull& coords,
					const G4double chargeIn,
					const G4int pdgID,
					const G4int turnsTaken,
					const G4int beamlineIndex)
{
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
  trackID.push_back(0);
  modelID = beamlineIndex;
  turnNumber.push_back(turnsTaken);
  S = (U) (coords.s / CLHEP::GeV);
  charge.push_back((int)(chargeIn / (G4double)CLHEP::eplus));
  FillPolarCoords(coords);
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

//#else
//void BDSOutputROOTEventSampler::SetBranchAddress(TTree *)
//{}
#endif

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
  std::vector<bool> result((unsigned long)n);
  if (!particleTable)
    {return result;}
  for (int i = 0; i < n; ++i)
    {result[i] = particleTable->IsIon(partID[i]);}
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

template <class U>
void BDSOutputROOTEventSampler<U>::FillMR()
{
  if (!particleTable)
    {return;}
  for (int i = 0; i < n; ++i)
    {// loop over all existing entires in the branch vectors
      auto& pid = partID[i];
      auto& pInfo = particleTable->GetParticleInfo(pid);
      mass.push_back(pInfo.mass);
      rigidity.push_back(pInfo.rigidity(energy[i], charge[i]));
    }
}

template <class U>
void BDSOutputROOTEventSampler<U>::FillMRK()
{
  if (!particleTable)
    {return;}
  for (int i = 0; i < n; ++i)
    {// loop over all existing entires in the branch vectors
      auto& pid = partID[i];
      auto& pInfo = particleTable->GetParticleInfo(pid);
      mass.push_back(pInfo.mass);
      rigidity.push_back(pInfo.rigidity(energy[i], charge[i]));
      kineticEnergy.push_back(particleTable->KineticEnergy(pid, energy[i]));
    }
}

template <class U>
void BDSOutputROOTEventSampler<U>::FillMRIK()
{
  if (!particleTable)
    {return;}
  for (int i = 0; i < n; ++i)
    {// loop over all existing entires in the branch vectors
      auto& pid = partID[i];
      if (particleTable->IsIon(pid))
        {
          auto& ionInfo = particleTable->GetIonInfo(pid);
          mass.push_back(ionInfo.mass);
          rigidity.push_back(ionInfo.rigidity(energy[i], charge[i]));
          isIon.push_back(true);
          ionA.push_back(ionInfo.a);
          ionZ.push_back(ionInfo.z);
          kineticEnergy.push_back(particleTable->KineticEnergy(pid, energy[i]));
        }
      else
        {// particle
          auto& pInfo = particleTable->GetParticleInfo(pid);
          mass.push_back(pInfo.mass);
          rigidity.push_back(pInfo.rigidity(energy[i], charge[i]));
          isIon.push_back(false);
          ionA.push_back(0);
          ionZ.push_back(0);
          kineticEnergy.push_back(particleTable->KineticEnergy(pid, energy[i]));
        }
    }
}

template <class U>
void BDSOutputROOTEventSampler<U>::FillMRI()
{
  if (!particleTable)
    {return;}
  for (int i = 0; i < n; ++i)
    {// loop over all existing entires in the branch vectors
      auto& pid = partID[i];
      if (particleTable->IsIon(pid))
	{
	  auto& ionInfo = particleTable->GetIonInfo(pid);
	  mass.push_back(ionInfo.mass);
	  rigidity.push_back(ionInfo.rigidity(energy[i], charge[i]));
	  isIon.push_back(true);
	  ionA.push_back(ionInfo.a);
	  ionZ.push_back(ionInfo.z);
	}
      else
	{// particle
	  auto& pInfo = particleTable->GetParticleInfo(pid);
	  mass.push_back(pInfo.mass);
	  rigidity.push_back(pInfo.rigidity(energy[i], charge[i]));
	  isIon.push_back(false);
	  ionA.push_back(0);
	  ionZ.push_back(0);
	}
    }
}

template class BDSOutputROOTEventSampler<float>;
template class BDSOutputROOTEventSampler<double>;
