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
#include "BDSOutputROOTEventTrajectoryPoint.hh"

ClassImp(BDSOutputROOTEventTrajectoryPoint)

BDSOutputROOTEventTrajectoryPoint::BDSOutputROOTEventTrajectoryPoint():
  partID(-1),
  trackID(-1),
  parentID(-1),
  parentIndex(-1),
  processType(-1),
  processSubType(-1),
  weight(-1.0),
  energyDeposited(-1.0),
  position(0,0,0),
  momentum(0,0,0),
  model(-1),
  time(0),
  positionLocal(0,0,0),
  momentumLocal(0,0,0),
  charge(0),
  kineticEnergy(0),
  turnsTaken(0),
  rigidity(0),
  mass(0),
  isIon(0),
  ionA(0),
  ionZ(0),
  nElectrons(0)
{;}

BDSOutputROOTEventTrajectoryPoint::BDSOutputROOTEventTrajectoryPoint(int      partIDIn,
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
								     int      nElectronsIn):
  partID(partIDIn),
  trackID(trackIDIn),
  parentID(parentIDIn),
  parentIndex(parentIndexIn),
  processType(processTypeIn),
  processSubType(processSubTypeIn),
  weight(weightIn),
  energyDeposited(energyDepositedIn),
  position(positionIn),
  momentum(momentumIn),
  model(modelIn),
  time(timeIn),
  positionLocal(positionLocalIn),
  momentumLocal(momentumLocalIn),
  charge(chargeIn),
  kineticEnergy(kineticEnergyIn),
  turnsTaken(turnsTakenIn),
  rigidity(rigidityIn),
  mass(massIn),
  isIon(isIonIn),
  ionA(ionAIn),
  ionZ(ionZIn),
  nElectrons(nElectronsIn)
{;}

BDSOutputROOTEventTrajectoryPoint::~BDSOutputROOTEventTrajectoryPoint()
{;}
