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
#include "BDSOutputROOTEventInfo.hh"

#include <ctime>

ClassImp(BDSOutputROOTEventInfo)

BDSOutputROOTEventInfo::BDSOutputROOTEventInfo():
  startTime(time_t()),
  stopTime(time_t()),
  duration(0),
  seedStateAtStart(""),
  aborted(false),
  primaryHitMachine(false),
  primaryAbsorbedInCollimator(false),
  memoryUsageMb(0),
  energyDeposited(0),
  energyDepositedWorld(0),
  energyDepositedTunnel(0),
  energyKilled(0),
  energyTotal(0)
{;}

BDSOutputROOTEventInfo::~BDSOutputROOTEventInfo()
{;}

void BDSOutputROOTEventInfo::Flush()
{
  startTime         = time_t();
  stopTime          = time_t();
  duration          = 0;
  seedStateAtStart  = "";
  aborted           = false;
  primaryHitMachine = false;
  primaryAbsorbedInCollimator = false;
  memoryUsageMb         = 0;
  energyDeposited       = 0;
  energyDepositedWorld  = 0;
  energyDepositedTunnel = 0;
  energyKilled          = 0;
  energyTotal           = 0;
}
