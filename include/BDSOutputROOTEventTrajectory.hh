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
#ifndef BDSOUTPUTROOTEVENTTRAJECTORY_H
#define BDSOUTPUTROOTEVENTTRAJECTORY_H

#include "Rtypes.h"
#include "TVector3.h"

#include <map>
#include <vector>

#ifndef __ROOTBUILD__
class BDSHitEnergyDeposition;
class BDSTrajectory;
template <class T> class G4THitsCollection;
typedef G4THitsCollection<BDSHitEnergyDeposition> BDSHitsCollectionEnergyDeposition;
#endif

class BDSAuxiliaryNavigator;

#if 0
0  fNotDefined,
  1  fTransportation,
    91 TRANSPORTATION,
    92 COUPLED_TRANSPORTATION
  2  fElectromagnetic,
    1  fCoulombScattering,
    2  fIonisation,
    3  fBremsstrahlung,
    4  fPairProdByCharged,
    5  fAnnihilation,
    6  fAnnihilationToMuMu,
    7  fAnnihilationToHadrons,
    8  fNuclearStopping,
    10 fMultipleScattering,
    11 fRayleigh,
    12 fPhotoElectricEffect,
    13 fComptonScattering,
    14 fGammaConversion,
    15 fGammaConversionToMuMu,
    21 fCerenkov,
    22 fScintillation,
    23 fSynchrotronRadiation,
    24 fTransitionRadiation
  3  fOptical,
    0 kCerenkov,
    1 kScintillation,
    2 kAbsorption,
    3 kRayleigh,
    4 kMieHG,
    5 kBoundary,
    6 kWLS,
    7 kNoProcess
  4  fHadronic,
    111 fHadronElastic,
    121 fHadronInelastic,
    131 fCapture,
    141 fFission,
    151 fHadronAtRest,
    152 fLeptonAtRest,
    161 fChargeExchange,
    210 fRadioactiveDecay
  5  fPhotolepton_hadron,
  6  fDecay,
    201 DECAY = 201,
    202 DECAY_WithSpin,
    203 DECAY_PionMakeSpin,
    210 DECAY_Radioactive,
    211 DECAY_Unknown,
    231 DECAY_External = 231
  7  fGeneral,
    401 STEP_LIMITER,
    402 USER_SPECIAL_CUTS,
    403 NEUTRON_KILLER
  8  fParameterisation,
  9  fUserDefined,
  10 fParallel,
    491 SBLN_UNKNOWN!
  11 fPhonon,
  12 fUCN
#endif
  
/**
 * @brief Structure to record a trajectory point.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOTEventTrajectoryPoint: public TObject
{
public :
  BDSOutputROOTEventTrajectoryPoint() :
          partID(-1), trackID(-1), parentID(-1), parentIndex(-1),
          processType(-1), processSubType(-1), weight(-1.0), energy(-1.0),
          position(0,0,0), momentum(0,0,0), model(-1), preTime(0) {};
  BDSOutputROOTEventTrajectoryPoint(int partIDIn, int trackIDIn, int parentIDIn, int parentIndexIn,
                                    int processTypeIn, int processSubTypeIn, double weightIn, double energyIn,
                                    TVector3 positionIn, TVector3 momentumIn, int modelIn, double preTimeIn) :
          partID(partIDIn), trackID(trackIDIn), parentID(parentIDIn), parentIndex(parentIndexIn),
          processType(processTypeIn), processSubType(processSubTypeIn), weight(weightIn), energy(energyIn),
          position(positionIn), momentum(momentumIn), model(modelIn), preTime(preTimeIn) {}
  virtual ~BDSOutputROOTEventTrajectoryPoint(){;}

  int partID;
  int trackID;
  int parentID;
  int parentIndex;
  int processType;
  int processSubType;
  double weight;
  double energy;
  TVector3 position;
  TVector3 momentum;
  int model;
  double preTime;

  ClassDef(BDSOutputROOTEventTrajectoryPoint,2);
};


/**
 * @brief Structure to record a trajectory.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOTEventTrajectory: public TObject
{
public:
  BDSOutputROOTEventTrajectory();
  virtual ~BDSOutputROOTEventTrajectory();
#ifndef __ROOTBUILD__
  void Fill(const std::map<BDSTrajectory*, bool> &trajectory);
  void Fill(const BDSHitsCollectionEnergyDeposition* phc);
#endif

  /// Required to find beamline index careful including in streamer.
  BDSAuxiliaryNavigator* auxNavigator; //! add comment to avoid warning (no need to make persistent, see issue #191)
  
  void Flush();
  void Fill(const BDSOutputROOTEventTrajectory* other);

  int n;
  std::vector<int>                   partID;
  std::vector<unsigned int>          trackID;
  std::vector<int>                   parentID;
  std::vector<int>                   parentIndex;
  std::vector<int>                   parentStepIndex;
  std::vector<int>                   primaryStepIndex;

  std::vector<std::vector<int>>      preProcessTypes;
  std::vector<std::vector<int>>      preProcessSubTypes;
  std::vector<std::vector<int>>      postProcessTypes;
  std::vector<std::vector<int>>      postProcessSubTypes;

  std::vector<std::vector<double>>   preWeights;
  std::vector<std::vector<double>>   postWeights;
  std::vector<std::vector<double>>   energies;

  std::vector<std::vector<TVector3>> trajectories;
  std::vector<std::vector<double>>   trajectoriesS;
  std::vector<std::vector<TVector3>> momenta;
  std::vector<std::vector<double>>   preT;

  std::vector<std::vector<int>>      modelIndicies;

  std::map<int, int>                 trackID_trackIndex;          // trackID to trackIndex 

  //  std::map<int, std::pair<int,int>>  trackIndex_trackProcess;     // trackProcess pair<trackIndex,trackProcessIndex>
  //  std::map<int, int>                 trackIndex_modelIndex;       // trackIndex to model index map
  //  std::map<int, std::vector<int>>    modelIndex_trackIndex;       // modelIndex to track index map
  //  std::pair<int,int>                 findParentProcess(int trackIndex);

  std::vector<BDSOutputROOTEventTrajectoryPoint> trackInteractions(int trackID);
  BDSOutputROOTEventTrajectoryPoint              primaryProcessPoint(int trackID);
  std::vector<BDSOutputROOTEventTrajectoryPoint> processHistory(int trackID);

  void printTrajectoryInfo(int trackID);

  friend std::ostream& operator<< (std::ostream& out, BDSOutputROOTEventTrajectory const &p);

  ClassDef(BDSOutputROOTEventTrajectory,3);
};


#endif
