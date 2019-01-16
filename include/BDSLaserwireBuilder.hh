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

/*
 LaserwireBuilder to make geometry in parallel world to beamline
 @author Siobhan Alden
 */
#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "BDSLaser.hh"
#include "BDSAcceleratorComponent.hh"

#ifndef BDSIM_BDSLASERWIREBUILDER_H
#define BDSIM_BDSLASERWIREBUILDER_H

class BDSLaserwireBuilder : public BDSAcceleratorComponent
{
public:
    BDSLaserwireBuilder(G4String nameIn,
                        G4double lengthIn,
                        G4double horizontalWidthIn,
                        G4double laserPositionIn,
                        G4ThreeVector laserdirectionIn,
                        BDSLaser* laserIn);
    virtual ~BDSLaserwireBuilder();


protected:

    virtual void BuildContainerLogicalVolume();

    virtual void Build();


    G4double horizontalWidth;
    G4double laserPosition;
    G4ThreeVector laserDirection;
    BDSLaser* laser;
    G4double laserRadius;

};


#endif //BDSIM_BDSLASERWIREBUILDER_H
