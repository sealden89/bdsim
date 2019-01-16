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
#include "BDSTransform3D.hh"

BDSTransform3D::BDSTransform3D(G4String nameIn,
			       G4double dxIn,
			       G4double dyIn,
			       G4double dzIn,
			       G4double dPhiIn,
			       G4double dThetaIn,
			       G4double dPsiIn):
  BDSAcceleratorComponent(nameIn,0,0,"transform3d"),
  dx(dxIn), dy(dyIn), dz(dzIn),
  dTheta(dThetaIn), dPsi(dPsiIn), dPhi(dPhiIn)
{;}

BDSTransform3D::~BDSTransform3D()
{;}
