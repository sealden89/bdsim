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
#include "BDSIonExcitationEngine.hh"
#include "BDSLaser.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSIonExcitationEngine::BDSIonExcitationEngine(BDSLaser* laserIn):laser(laserIn)
{;}

BDSIonExcitationEngine::~BDSIonExcitationEngine()
{;}

G4double BDSIonExcitationEngine::CrossSection(G4double photonEnergy)
{
 G4double crossSection0 = 4.62e-12; // hard coded as mm for now but need to consider how to get this for each beam
 G4double laserOmegaPrime = photonEnergy/CLHEP::h_Planck;
 G4double laserOmega0 = (CLHEP::h_Planck*CLHEP::c_light)/laser->Wavelength();
 G4double lifetimeOfExcitation = 74e-12*CLHEP::second; //hard coded for now again

 //G4double crossSection = crossSection0/(1.0+4*lifetimeOfExcitation*())

 return 0;



}


