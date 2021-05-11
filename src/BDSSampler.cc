/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSBeamPipe.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSSampler.hh"

#include "G4String.hh"
#include "G4LogicalVolume.hh"

BDSSampler::BDSSampler(G4String nameIn):
  BDSGeometryComponent(nullptr, nullptr),
  name(nameIn)
{;}

void BDSSampler::CommonConstruction()
{
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       BDSMaterials::Instance()->GetMaterial("G4_Galactic"),
					       GetName() + "_lv");
  
  containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->VisibleDebugVisAttr());
}
