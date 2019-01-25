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
#include "BDSLaser.hh"
#include "globals.hh" // geant4 types / globals

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSLaser::BDSLaser(G4double wavelengthIn,
                   G4double m2In,
                   G4double pulseDurationIn,
                   G4double pulseEnergyIn,
                   G4double waistIn):
  wavelength(wavelengthIn),
  m2(m2In),
  pulseDuration(pulseDurationIn),
  pulseEnergy(pulseEnergyIn),
  waist(waistIn)
{
  peakPower = pulseEnergy / pulseDuration;
}

BDSLaser::~BDSLaser()
{;}

BDSLaser::BDSLaser(const BDSLaser& laser)
{
  waist         = laser.wavelength;
  m2            = laser.m2;
  pulseDuration = laser.pulseDuration;
  pulseEnergy   = laser.pulseEnergy;
  waist         = laser.waist;
}

const G4double BDSLaser::RayleighRange()
{
  return (CLHEP::pi * waist * waist)/(wavelength * m2);
}

const G4double BDSLaser::Width(G4double particlePosition)
{
  return waist*sqrt(1.0-std::pow(particlePosition/RayleighRange(),2.0));
}

// this will not work because of the position needed
// need to call the particle position in coordinates relative to the laser vector to establish laser width and intensity

const G4double BDSLaser::Intensity(G4double radius, G4double distanceFromFocus)
{
  G4double width2 = std::pow(Width(distanceFromFocus),2);
  return (2.0*peakPower)/(CLHEP::pi * width2) * exp((-2.0*std::pow(radius,2))/(width2));
}

const G4double BDSLaser::Radius()
{
  return std::sqrt((waist*log(1.0/(CLHEP::e_squared)))/-2.0);
}

const G4double BDSLaser::PhotonEnergy(G4double particleGamma, G4double overlapAngle, G4double particleBeta)
{
  return particleGamma*((CLHEP::h_Planck*CLHEP::c_light)/wavelength)
         *(1-particleBeta*std::cos(overlapAngle*CLHEP::radian));
}