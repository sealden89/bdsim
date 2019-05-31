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
#ifndef BDSOUTPUTROOTEVENTSAMPLER_H
#define BDSOUTPUTROOTEVENTSAMPLER_H

#include "TROOT.h"
#include <vector>

class BDSOutputROOTGeant4Data;

#ifndef __ROOTBUILD__ 
#include "globals.hh"
class BDSParticleCoordsFull;
class BDSHitSampler;
#endif

#include "TObject.h"
#include "TTree.h"

/**
 * @brief Information stored per sampler per event.
 * 
 * @author Stewart Boogert
 */

template<class U> class BDSOutputROOTEventSampler: public TObject
{
public:
  std::string samplerName; //|| Don't split the header
  
  int                n;
  std::vector<U>     energy;
  std::vector<U>     x;
  std::vector<U>     y;
  U                  z;
  std::vector<U>     xp;
  std::vector<U>     yp;
  std::vector<U>     zp;
  std::vector<U>     T;

  std::vector<U>     weight;
  std::vector<int>   partID;
  std::vector<int>   parentID;
  std::vector<int>   trackID;
  int                modelID;
  std::vector<int>   turnNumber;

  U                  S;

  std::vector<U>     r;
  std::vector<U>     rp;
  std::vector<U>     phi;
  std::vector<U>     phip;

  /// @{ These are not filled by default.
  std::vector<int>     charge;
  std::vector<U>       kineticEnergy;
  std::vector<U>       mass;
  std::vector<U>       rigidity;
  std::vector<bool>    isIon;
  std::vector<int>     ionA;
  std::vector<int>     ionZ;
  std::vector<int>     nElectrons;
  /// @}

  /// @{ Function to calculate on the fly the parameters.
  std::vector<U>       getKineticEnergy();
  std::vector<U>       getMass();
  std::vector<U>       getRigidity();
  std::vector<bool>    getIsIon();
  std::vector<int>     getIonA();
  std::vector<int>     getIonZ();
  /// @}  
  
  BDSOutputROOTEventSampler();
  explicit BDSOutputROOTEventSampler(std::string samplerNameIn);
  virtual ~BDSOutputROOTEventSampler();
#ifndef __ROOTBUILD__
  void Fill(const BDSHitSampler* hit,
	    G4bool storeMass          = false,
	    G4bool storeCharge        = false,
	    G4bool storePolarCoords   = false,
	    G4bool storeElectrons     = false,
	    G4bool storeRigidity      = false,
	    G4bool storeKineticEnergy = false);
  /// Used for filling primary coordinates only.
  void Fill(const BDSParticleCoordsFull& coords,
	    const G4double charge,
	    const G4int    pdgID,
	    const G4int    turnsTaken,
	    const G4int    beamlineIndex,
	    const G4int    nElectronsIn,
	    const G4double massIn,
	    const G4double rigidityIn);
  void FillPolarCoords(const BDSParticleCoordsFull& coords); ///< Calculate polar coords and fill.
#endif
  void Fill(const BDSOutputROOTEventSampler<U>* other);

  /// @{ Calculate and fill calculated variables.
  inline void FillIon() {isIon = getIsIon(); ionA = getIonA(); ionZ = getIonZ();}
  /// @}
  
  void SetBranchAddress(TTree *);
  virtual void Flush();  ///< Clean Sampler

  static BDSOutputROOTGeant4Data* particleTable;

  ClassDef(BDSOutputROOTEventSampler,3);
};

#endif
