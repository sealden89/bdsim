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

