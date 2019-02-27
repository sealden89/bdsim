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

BDSPhotoDetachmentEngine::BDSPhotoDetachmentEngine()
{

}

BDSPhotoDetachmentEngine::~BDSPhotoDetachmentEngine()
{
    ;
}

const G4double BDSPhotoDetachmentEngine::CrossSection(G4double photonEnergy)
{
    G4double lambdaShift = ((CLHEP::h_Planck*CLHEP::c_light)/(photonEnergy*CLHEP::electron_charge))*1.0e9;
    G4double crossSectionFit = fitCoefficient1*std::pow(lambdaShift,4.0)
                             - fitCoefficient2*std::pow(lambdaShift,3.0)
                             + fitCoefficient3*std::pow(lambdaShift,2.0)
                             + fitCoefficient4*lambdaShift
                             - fitCoefficient5;

    return crossSectionFit*1e-17*1e-4*CLHEP::meter2;
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
    G4double electronEnergy = gamma*electronMass;
    G4double electronMomentumMagnitude = std::sqrt(electronEnergy*electronEnergy-electronMass*electronMass);
    G4ThreeVector electronMomentum;
    G4double electronPx = (ionMomentum[0]/ionMomentumMagnitude)*electronMomentumMagnitude;
    G4double electronPy = (ionMomentum[1]/ionMomentumMagnitude)*electronMomentumMagnitude;
    G4double electronPz = (ionMomentum[2]/ionMomentumMagnitude)*electronMomentumMagnitude;
    electronMomentum.set(electronPx, electronPy,electronPz);
    SetElectronEnergy(electronEnergy);
    SetElectronMomentum(electronMomentum);

    G4double hydrogenEnergy = gamma*hydrogenMass;
    G4double hydrogenMomentumMagnitude = std::sqrt(hydrogenEnergy*hydrogenEnergy-hydrogenMass*hydrogenMass);
    G4ThreeVector hydrogenMomentum;
    G4double hydrogenPx = (ionMomentum[0]/ionMomentumMagnitude)*hydrogenMomentumMagnitude;
    G4double hydrogenPy = (ionMomentum[1]/ionMomentumMagnitude)*hydrogenMomentumMagnitude;
    G4double hydrogenPz = (ionMomentum[2]/ionMomentumMagnitude)*hydrogenMomentumMagnitude;
    electronMomentum.set(hydrogenPx, hydrogenPy,hydrogenPz);
    SetElectronEnergy(hydrogenEnergy);
    SetElectronMomentum(hydrogenMomentum);

}

