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

#include "BDSLaserwireBuilder.hh"

BDSLaserwireBuilder::BDSLaserwireBuilder(G4String nameIn,
                                         G4double lengthIn,
                                         G4double laserPositionIn,
                                         G4ThreeVector laserDirectionIn,
                                         BDSLaser* laserIn):
        BDSAcceleratorComponent(nameIn, lengthIn, 0, "laserwire"),
        laserPosition(laserPositionIn),
        laserDirection(laserDirectionIn),
        laser(laserIn)

{;}

BDSLaserwireBuilder::~BDSLaserwireBuilder()
{;}

void BDSLaserwireBuilder::BuildContainerLogicalVolume()
{
    // need a bunch of if and errors
    containerSolid = new G4Tubs(name+"_container_solid",
                                0,
                                laser->GetRadius(),
                                laser->GetLaserFocus(),
                                0,
                                2.0*CLHEP::pi);
}

void BDSLaserwireBuilder::Build()
{
    BDSAcceleratorComponent::Build();


}