/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#ifndef BDSOUTPUTROOTEVENTTRAJECTORYPOINT_H
#define BDSOUTPUTROOTEVENTTRAJECTORYPOINT_H
#include "Rtypes.h"
#include "TVector3.h"

/**
 * @brief Structure to record a trajectory point.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOTEventTrajectoryPoint: public TObject
{
public:
  BDSOutputROOTEventTrajectoryPoint();
  BDSOutputROOTEventTrajectoryPoint(int      partIDIn,
				    int      trackIDIn,
				    int      parentIDIn,
				    int      parentIndexIn,
				    int      processTypeIn,
				    int      processSubTypeIn,
				    double   weightIn,
				    double   energyDepositedIn,
				    TVector3 positionIn,
				    TVector3 momentumIn,
				    int      modelIn,
				    double   timeIn,
				    TVector3 positionLocalIn,
				    TVector3 momentumLocalIn,
				    double   chargeIn,
				    double   kineticEnergyIn,
				    int      turnsTakenIn,
				    double   rigidityIn,
				    double   massIn,
				    bool     isIonIn,
				    int      ionAIn,
				    int      ionZIn,
				    int      nElectronsIn);
  virtual ~BDSOutputROOTEventTrajectoryPoint();

  int      partID;
  int      trackID;
  int      parentID;
  int      parentIndex;
  int      processType;
  int      processSubType;
  double   weight;
  double   energyDeposited;
  TVector3 position;
  TVector3 momentum;
  int      model;
  double   time;
  TVector3 positionLocal;
  TVector3 momentumLocal;
  double   charge;
  double   kineticEnergy;
  int      turnsTaken;
  double   rigidity;
  double   mass;
  bool     isIon;
  int      ionA;
  int      ionZ;
  int      nElectrons;

  ClassDef(BDSOutputROOTEventTrajectoryPoint,3);
};

#endif
