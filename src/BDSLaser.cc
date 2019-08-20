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

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSLaser::BDSLaser(G4double wavelengthIn,
                   G4double m2In,
                   G4double pulseDurationIn,
                   G4double pulseEnergyIn,
                   G4double sigma0In,
                   G4double laserIPTimeIn):
  wavelength(wavelengthIn),
  m2(m2In),
  pulseDuration(pulseDurationIn),
  pulseEnergy(pulseEnergyIn),
  sigma0(sigma0In),
  laserIPTime(laserIPTimeIn)
{
  if(!BDS::IsFinite(sigma0In))
    {throw BDSException(__METHOD_NAME__, "Laser waist sigma0 is zero.");}
  
  peakPower = pulseEnergy / pulseDuration;

  rayleighRange = (CLHEP::pi * (2.0*sigma0)*(2.0*sigma0)) / (wavelength * m2);
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
  laserIPTime   = laser.LaserIPTime();
}

G4double BDSLaser::W(G4double z) const
{
  return W0()*std::sqrt(1.0+std::pow(z/rayleighRange,2.0));
}

G4double BDSLaser::Intensity(G4double x, G4double y, G4double z, G4double /*t*/) const
{
  return (2.0*peakPower)/(CLHEP::pi*W(z)*W(z)) * std::exp(-(2.0*(x*x+y*y))/(W(z)*W(z)));
}
G4double BDSLaser::Intensity(G4ThreeVector xyz, G4double /*t*/) const
{
  return (2.0*peakPower)/(CLHEP::pi*W(xyz.z())*W(xyz.z())) *
          std::exp(-(2.0*(xyz.x()*xyz.x()+xyz.y()*xyz.y()))/(W(xyz.z())*W(xyz.z())));
}

G4double BDSLaser::Radius() const
{
  return std::sqrt((W0()*std::log(1.0/(CLHEP::e_squared)))/-2.0);
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
  return (m2*wavelength)/(2.0*CLHEP::pi*W0());
}

G4double BDSLaser::TemporalProfileGaussian(G4double particleGlobalTime, G4double particleZCoord) const
{


  if(laserIPTime==0)
  {
    return 1.0;
  }
  else
  {
    G4double mu = (particleGlobalTime-laserIPTime)*CLHEP::nanosecond; // can be negative - locates the peak of the pulse in time for a give particleGlobalTime
    G4double sigmaT = pulseDuration/(2.0 * std::sqrt(2.0 * std::log(2.0))) ;
    return std::exp(-((particleZCoord/CLHEP::c_light - mu)*(particleZCoord/CLHEP::c_light-mu)) / (2.0 * sigmaT * sigmaT));
  }
}



G4String BDSLaser::GetLaserColour()
{
  auto it = std::lower_bound(wavelengths.begin(),wavelengths.end(),wavelength);
  G4int index = std::distance(wavelengths.begin(),it);
  return colours[index];
  //std::binary_search(wavelengths.begin(),wavelengths.end(),wavelength)

 /* G4String laserColour="";
  if(wavelength<=340.0*CLHEP::nanometer){laserColour="magenta";}
  else if(wavelength<=425.0*CLHEP::nanometer){laserColour="decapole";}
  else if(wavelength<=445.0*CLHEP::nanometer){laserColour="blue";}
  else if(wavelength<=520.0*CLHEP::nanometer){laserColour="muonspoiler";}
  else if(wavelength<=565.0*CLHEP::nanometer){laserColour="green";}
  else if(wavelength<=590.0*CLHEP::nanometer){laserColour="yellow";}
  else if(wavelength<=625.0*CLHEP::nanometer){laserColour="solenoid";}
  else if(wavelength<=740.0*CLHEP::nanometer){laserColour="red";}
  else{laserColour="quadrupole";}
  return laserColour;
*/
}