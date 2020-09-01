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
#include "BDSFieldMagInterpolated4D.hh"
#include "BDSInterpolator4D.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

BDSFieldMagInterpolated4D::BDSFieldMagInterpolated4D(BDSInterpolator4D*   interpolatorIn,
						     const G4Transform3D& offset,
						     G4double             scalingIn):
  BDSFieldMagInterpolated(interpolatorIn->Extent(), offset, scalingIn),
  interpolator(interpolatorIn)
{;}

BDSFieldMagInterpolated4D::~BDSFieldMagInterpolated4D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldMagInterpolated4D::GetField(const G4ThreeVector& position,
						  const G4double       t) const
{
  return interpolator->GetInterpolatedValue(position[0], position[1], position[2], t) * Scaling();
}
