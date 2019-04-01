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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSLaser.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSLaser::BDSLaser(G4double wavelengthIn,
                   G4double m2In,
                   G4double pulseDurationIn,
                   G4double pulseEnergyIn,
                   G4double sigma0In):
  wavelength(wavelengthIn),
  m2(m2In),
  pulseDuration(pulseDurationIn),
  pulseEnergy(pulseEnergyIn),
  sigma0(sigma0In)
{
  peakPower = pulseEnergy / pulseDuration;

  rayleighRange = (CLHEP::pi * std::pow(W0(),2)) / (wavelength * m2);
}

BDSLaser::~BDSLaser()
{;}

BDSLaser::BDSLaser(const BDSLaser& laser)
{
  wavelength    = laser.wavelength;
  m2            = laser.m2;
  pulseDuration = laser.pulseDuration;
  pulseEnergy   = laser.pulseEnergy;
  sigma0        = laser.sigma0;
  peakPower     = laser.peakPower;
  rayleighRange = laser.rayleighRange;
}

G4double BDSLaser::W(G4double z) const
{
  return W0()*std::sqrt(1.0+std::pow(z/rayleighRange,2.0));
}

// this will not work because of the position needed
// need to call the particle position in coordinates relative to the laser vector to establish laser width and intensity

G4double BDSLaser::Intensity(G4double radius, G4double distanceFromFocus) const
{
  G4double w2 = std::pow(W(distanceFromFocus),2);
  return (2.0*peakPower)/(CLHEP::pi*w2) * std::exp((-2.0*std::pow(radius,2))/(w2));
}

G4double BDSLaser::Radius() const
{
  return std::sqrt((W0()*log(1.0/(CLHEP::e_squared)))/-2.0);
}

G4double BDSLaser::PhotonEnergy(G4double particleGamma,
				G4double overlapAngle,
				G4double particleBeta) const
{
  return particleGamma*((CLHEP::h_Planck*CLHEP::c_light)/wavelength)
         *(1-particleBeta*std::cos(overlapAngle*CLHEP::radian));
}

G4double BDSLaser::HyperbolicAngle() const
{
  if(!BDS::IsFinite(W0()))
    {throw BDSException(__METHOD_NAME__, "Laser waist, w0, or sigma0 is zero.");}
  return (m2*wavelength)/(CLHEP::pi*W0());
}

/*
G4double BDSLaser::WavelengthShift(G4double laserAngleIn)
{;}
*/
