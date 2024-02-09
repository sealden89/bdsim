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
#include "BDSExtent.hh"
#include "BDSFieldMagInterpolated.hh"
#include "BDSInterpolator.hh"

#include <limits>

BDSFieldMagInterpolated::BDSFieldMagInterpolated(const BDSInterpolator* interpolator,
						 const G4Transform3D&   offset,
						 G4double               scalingIn):
  BDSFieldMag(offset),
  timeVarying(false),
  scaling(scalingIn),
  extentNoOffset(BDSExtent()),
  smallestSpatialStep(std::numeric_limits<double>::max())
{
  if (interpolator)
    {
      extentNoOffset = interpolator->Extent();
      smallestSpatialStep = interpolator->SmallestSpatialStep();
      timeVarying = interpolator->TimeVarying();
    }
}
