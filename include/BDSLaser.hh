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
 Describe the function here

 @author Siobhan Alden
 */
#ifndef BDSLASER_H
#define BDSLASER_H

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include <math.h>
#include <vector>
#include <cmath>

class BDSLaser
{
public:
    BDSLaser(G4double alaserWavelength,
             G4double alaserM2,
             G4double alaserPulse,
             G4double alaserEnergy,
             G4double alaserFocus,
             G4double alaserLensDiameter,
             G4double alaserTime);

    //functions needed intensity, waist, beam width, rayleigh length
    G4double LaserWaist();
    G4double LaserRayleigh();
    //this needs to be called based upon particle coordinates
    G4double LaserWidth(G4double particlePosition);
    G4double LaserIntensity(G4double radius,G4double particlePosition);
    G4double GetRadius();

    //getters & setters
    G4double GetLaserM2();
    G4double GetLaserPulse();
    G4double GetLaserEnergy();
    G4double GetLaserFocus();
    G4double GetLaserLensDiameter();
    G4double GetLaserTime();



private:
    BDSLaser();
    G4double laserPeakPower;
    G4double laserWavelength;
    G4double laserM2;
    G4double laserPulse;
    G4double laserEnergy;
    G4double laserFocus;
    G4double laserLensDiameter;
    G4double laserTime;

};

#endif