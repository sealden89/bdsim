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
#ifndef BDSTRAJECTORYFILTER_H
#define BDSTRAJECTORYFILTER_H

#include "BDSTypeSafeEnum.hh"

/**
 * @brief Type definition for trajectory filters.
 *
 * Note: no Geant4 classes are used by this as it can be called by the output.
 * 
 * @author Laurie Nevay
 */

struct trajectoryfiltertypes_def
{
  enum type {primary, depth, particle, energyThreshold, sampler, elossSRange,
	     transportation, minimumZ, maximumR, connect};
};

typedef BDSTypeSafeEnum<trajectoryfiltertypes_def,int> BDSTrajectoryFilter;

namespace BDS
{
  const static int NTrajectoryFilters = 10;
  BDSTrajectoryFilter BDSTrajectoryFilterEnumOfIndex(int i);
}

#endif
