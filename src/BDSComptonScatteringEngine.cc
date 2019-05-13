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
#include "BDSComptonScatteringEngine.hh"
#include "G4Electron.hh"
#include "G4Proton.hh"
#include "Randomize.hh"
#include "BDSException.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSComptonScatteringEngine::BDSComptonScatteringEngine()
{;}

BDSComptonScatteringEngine::~BDSComptonScatteringEngine()
{;}

G4double BDSComptonScatteringEngine::CrossSection(G4double photonEnergyIn, G4int partIDIn)
{
    G4double particleMass = 0;
    G4double particleRadius = 0;
    if(abs(partIDIn)==11){
        particleMass = G4Electron::ElectronDefinition()->GetPDGMass();
        particleRadius = (CLHEP::e_squared)/(4*CLHEP::pi*CLHEP::epsilon0*particleMass);
    }
    else if (partIDIn == 18){
        particleMass = G4Proton::ProtonDefinition()->GetPDGMass();
        particleRadius = (CLHEP::e_squared)/(4*CLHEP::pi*CLHEP::epsilon0*particleMass);
    }

    G4double x = photonEnergyIn/particleMass;
    G4double crossSectionThomson =  (8/3)*CLHEP::pi*particleRadius*particleRadius;
    G4double crossSection = 0;
    if(x<1)
    {
        crossSection = crossSectionThomson*(1+2*x+(26/5)*x*x);
        return crossSection;
    }
    else if (x>=1)
    {
        crossSection= (3/8)*crossSectionThomson*(1/x)*(std::log(2*x)+1/2);
        return crossSection;
    }

}


void BDSComptonScatteringEngine::PerformCompton(G4int partiIDIn, G4ThreeVector boost)
{
    G4double particleMass;
    if(abs(partiIDIn==11)){ particleMass = G4Electron::ElectronDefinition()->GetPDGMass(); }
    else if (partiIDIn ==18){ particleMass = G4Proton::ProtonDefinition()->GetPDGMass(); }

    G4double theta = G4UniformRand()*CLHEP::twopi;
    G4double phi = G4UniformRand()*CLHEP::twopi;
    G4double scatteredGammaEnergy = incomingGamma.e()/(1+(incomingGamma.e()/particleMass)*(1-std::cos(theta)));
    G4ThreeVector scatteredGammaUnitVector;
    G4double sinTheta = std::sin(theta);
    G4double cosTheta = std::cos(theta);
    G4double sinPhi = std::sin(phi);
    G4double cosPhi = std::cos(phi);
    scatteredGammaUnitVector.set(sinTheta*cosPhi,sinTheta*sinPhi,cosTheta);
    scatteredGamma.setPx(scatteredGammaUnitVector.x()*scatteredGammaEnergy);
    scatteredGamma.setPy(scatteredGammaUnitVector.y()*scatteredGammaEnergy);
    scatteredGamma.setPz(scatteredGammaUnitVector.z()*scatteredGammaEnergy);
    scatteredGamma.setE(scatteredGammaEnergy);

    scatteredElectron.setE(incomingGamma.e()+incomingElectron.e()-scatteredGammaEnergy);
    scatteredElectron.setPx(-1.0*scatteredGamma.px());
    scatteredElectron.setPy(-1.0*scatteredGamma.py());
    scatteredElectron.setPz(-1.0*scatteredGamma.pz());


    scatteredElectron.boost(boost);
    scatteredGamma.boost(boost);


}
