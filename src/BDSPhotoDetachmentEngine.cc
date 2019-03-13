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
#include "BDSPhotoDetachmentEngine.hh"
#include "G4Electron.hh"
#include "G4Hydrogen.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSPhotoDetachmentEngine::BDSPhotoDetachmentEngine():
  gamma(0),
  ionMomentum(0),
  ionEnergy(0),
  electronMomentum(0),
  hydrogenMomentum(0),
  hydrogenEnergy(0),
  photonEnergy(0)
{;}

BDSPhotoDetachmentEngine::~BDSPhotoDetachmentEngine()
{;}

G4double BDSPhotoDetachmentEngine::CrossSection(G4double photonEnergyIn)
{
    G4double photonEnergyJoules = photonEnergyIn*(1/CLHEP::joule);
    G4double lambdaShift = ((CLHEP::h_Planck*(1/CLHEP::joule)*(1/CLHEP::second)*CLHEP::c_light*CLHEP::ms)/(photonEnergyJoules));
    G4double lambda_nm = lambdaShift*1.0e9;
    G4double crossSectionFit = fitCoefficient1*(lambda_nm*lambda_nm*lambda_nm*lambda_nm)
                             - fitCoefficient2*(lambda_nm*lambda_nm*lambda_nm)
                             + fitCoefficient3*(lambda_nm*lambda_nm)
                             + fitCoefficient4*lambda_nm
                             - fitCoefficient5;

    return crossSectionFit*1e-17*1e-4;
}

void BDSPhotoDetachmentEngine::PerformPhotoDetachment()
{

    G4double ionMomentumMagnitude = std::sqrt(ionMomentum[0]*ionMomentum[0]+
                                              ionMomentum[1]*ionMomentum[1]+
                                              ionMomentum[2]*ionMomentum[2]);
    G4Hydrogen* hydrogen = G4Hydrogen::Definition();
    G4double hydrogenMass = hydrogen->GetPDGMass();
    //electron calculations below
    G4Electron* electron = G4Electron::ElectronDefinition();
    G4double electronMass = electron->GetPDGMass();

    //add some kinematics....
    //    G4double electronEnergy = gamma*electronMass;
    electronEnergy = gamma*electronMass; // LN - fix shadow warning
    G4double electronMomentumMagnitude = std::sqrt(electronEnergy*electronEnergy-electronMass*electronMass);
    // G4ThreeVector electronMomentum; // LN - fix shadown warning
    G4double electronPx = (ionMomentum[0]/ionMomentumMagnitude)*electronMomentumMagnitude;
    G4double electronPy = (ionMomentum[1]/ionMomentumMagnitude)*electronMomentumMagnitude;
    G4double electronPz = (ionMomentum[2]/ionMomentumMagnitude)*electronMomentumMagnitude;
    electronMomentum.set(electronPx, electronPy,electronPz);
    //SetElectronEnergy(electronEnergy); // LN - these are unnecessary if we use
    //SetElectronMomentum(electronMomentum); // LN - the member variables directly

    //G4double hydrogenEnergy = gamma*hydrogenMass; // LN - fix shadow warning
    hydrogenEnergy = gamma*hydrogenMass;
    G4double hydrogenMomentumMagnitude = std::sqrt(hydrogenEnergy*hydrogenEnergy-hydrogenMass*hydrogenMass);
    //G4ThreeVector hydrogenMomentum; // LN - fix shadow warning
    G4double hydrogenPx = (ionMomentum[0]/ionMomentumMagnitude)*hydrogenMomentumMagnitude;
    G4double hydrogenPy = (ionMomentum[1]/ionMomentumMagnitude)*hydrogenMomentumMagnitude;
    G4double hydrogenPz = (ionMomentum[2]/ionMomentumMagnitude)*hydrogenMomentumMagnitude;
    electronMomentum.set(hydrogenPx, hydrogenPy,hydrogenPz); // LN - check this
    //SetElectronEnergy(hydrogenEnergy); // LN - unnecessary as use member varibales directly
    //SetElectronMomentum(hydrogenMomentum);
}

