/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSFactoryBase.hh"
#include "BDSGlobalConstants.hh"

#include <set>

BDSFactoryBase::BDSFactoryBase()
{
  BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  lengthSafety       = g->LengthSafety();
  lengthSafetyLarge  = g->LengthSafetyLarge();
  checkOverlaps      = g->CheckOverlaps();
  visDebug           = g->VisDebug();
  nSegmentsPerCircle = g->NSegmentsPerCircle();
  containerVisAttr   = g->ContainerVisAttr();
  defaultUserLimits  = g->DefaultUserLimits();

  // initialise variables and pointers that'll be used by the factory
  FactoryBaseCleanUp();
}

void BDSFactoryBase::FactoryBaseCleanUp()
{
  allLogicalVolumes.clear();
  allPhysicalVolumes.clear();
  allRotationMatrices.clear();
  allUserLimits.clear();
  allSolids.clear();
  allVisAttributes.clear();
}
