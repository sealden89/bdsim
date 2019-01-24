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
#ifndef BDSLASER_H
#define BDSLASER_H

#include "globals.hh" // geant4 types / globals

/**
 * @brief Describe the function here
 *
 * @author Siobhan Alden
 */

class BDSLaser
{
public:
  BDSLaser(G4double laserWavelengthIn,
	   G4double laserM2In,
	   G4double laserPulseDurationIn,
	   G4double laserPulseEnergyIn,
	   G4double laserWaist);
  /// Copy constructor

  ~BDSLaser();

    BDSLaser(const BDSLaser &laser);

    //functions needed intensity, waist, beam width, rayleigh length
    G4double LaserRayleigh();
    //this needs to be called based upon particle coordinates
    G4double LaserWidth(G4double particlePosition);
    G4double LaserIntensity(G4double radius,G4double distanceFromFocus);
    G4double GetRadius();
    inline G4double Wavelength() const {return laserWavelength;}
    //getters & setters
    inline G4double M2() const {return laserWavelength;}
    inline G4double PulseDuration() const {return laserPulseDuration;}
    inline G4double PulseEnergy() const {return laserPulseEnergy;}
    inline G4double Waist() const {return laserWaist;}



protected:
    BDSLaser() = delete;
    G4double laserPeakPower;
    G4double laserWavelength;
    G4double laserM2;
    G4double laserPulseDuration;
    G4double laserPulseEnergy;
    G4double laserWaist;
};

#endif
