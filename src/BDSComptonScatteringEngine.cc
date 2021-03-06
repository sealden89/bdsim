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
#include "BDSComptonScatteringEngine.hh"

#include "globals.hh"
#include "G4Electron.hh"
#include "G4Proton.hh"
#include "G4RandomDirection.hh"
#include "Randomize.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSComptonScatteringEngine::BDSComptonScatteringEngine():
  particleMass(0),
  particleRadius(0),
  partID(0)
{;}

BDSComptonScatteringEngine::~BDSComptonScatteringEngine()
{;}

void BDSComptonScatteringEngine::SetParticle(G4int partIDIn)
{
  partID = partIDIn;
  if (std::abs(partID) == 11)
    {particleMass = G4Electron::ElectronDefinition()->GetPDGMass();}
  else if (partID == 2212)
    {particleMass = G4Proton::ProtonDefinition()->GetPDGMass();}
  // else particle mass just left as whatever it was before?
  particleRadius = (CLHEP::e_squared) / (4 * CLHEP::pi * CLHEP::epsilon0 * particleMass);
}

G4double BDSComptonScatteringEngine::CrossSection(G4double photonEnergyIn, G4int partIn)
{
  SetParticle(partIn);
  G4double e1 = photonEnergyIn / particleMass;

  G4double crossSectionThomson = (8.0/3.0)*CLHEP::pi*particleRadius*particleRadius;

  G4double p1 = ((1 + e1) / std::pow(e1,3)) * ( 2*e1 * (1 + e1) / (1 + 2*e1) - std::log(1 + 2*e1));
  G4double p2 = (1. / (2*e1)) * std::log(1 + 2*e1);
  G4double p3 = (1 + 3*e1) / std::pow((1 + 2*e1), 2);
  G4double ratio = 0.75*(p1 + p2 - p3);
  G4double crossSection = crossSectionThomson*ratio;

  return crossSection;
}

void BDSComptonScatteringEngine::PerformCompton(const G4ThreeVector& boost,G4int partIn)
{
  SetParticle(partIn);
  G4ThreeVector scatteredGammaUnitVector = MCMCTheta();
  G4double theta = std::acos(scatteredGammaUnitVector.z());
  G4double scatteredGammaEnergy = incomingGamma.e()/(1+(incomingGamma.e()/particleMass)*(1-std::cos(theta)));
  
  scatteredGamma.setVect(scatteredGammaUnitVector * scatteredGammaEnergy);
  scatteredGamma.setE(scatteredGammaEnergy);
  
  scatteredElectron.setE(incomingGamma.e()+incomingElectron.e()-scatteredGammaEnergy);
  scatteredElectron.setVect(-1.0*scatteredGamma.vect());
  
  scatteredElectron.boost(boost);
  scatteredGamma.boost(boost);
}

G4ThreeVector BDSComptonScatteringEngine::MCMCTheta()
{
  G4ThreeVector randomDirection = G4RandomDirection();
  // G4double theta = randomDirection.phi(); //< equivalent to next line - note phi() uses atan2 - do we really want atan here?
  G4double theta = std::atan(randomDirection.y()/randomDirection.z());
  G4double KNTheta = KleinNishinaDifferential(theta);
  G4double KNMax = KleinNishinaDifferential(0);
  G4double KNRandom = G4UniformRand()*KNMax;

  // bool ? if true : if false
  return KNTheta > KNRandom ? randomDirection : MCMCTheta();
}

G4double BDSComptonScatteringEngine::KleinNishinaDifferential(G4double theta)
{
  G4double E0 = incomingGamma.e();
  G4double Ep = E0 / (1.0+(E0/particleMass) * (1.0-std::cos(theta)) );
  return 0.5 * particleRadius * particleRadius * (Ep/E0) * (E0/Ep) * ((E0/Ep)+(Ep/E0)-std::sin(theta)*std::sin(theta));
}

