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
#ifndef BDSINTERPOLATORTYPE_H
#define BDSINTERPOLATORTYPE_H

#include "BDSTypeSafeEnum.hh"

#include "globals.hh" // geant4 types / globals

/**
 * @brief Type definition for interpolator.
 * 
 * @author Laurie Nevay
 */

struct interpolatortypes_def
{
  enum type
    {
      none,
      nearest1d, linear1d, cubic1d,
      nearest2d, linear2d, cubic2d,
      nearest3d, linear3d, cubic3d,
      nearest4d, linear4d, cubic4d
    };
};

typedef BDSTypeSafeEnum<interpolatortypes_def,int> BDSInterpolatorType;

namespace BDS
{
  /// Function that determines enum from string (case-insensitive).
  BDSInterpolatorType DetermineInterpolatorType(G4String interpolatorType);
}

#endif
