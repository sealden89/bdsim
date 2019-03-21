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
#ifndef BDSIONEXCITATIONENGINE_H
#define BDSIONEXCITATIONENGINE_H

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"

class BDSIonExcitationEngine
{
public:
    BDSIonExcitationEngine();
    ~BDSIonExcitationEngine();
    G4double CrossSection(G4double photonEnergy);
    void PerformPhotoDetachment();
  //setters
    inline void SetIonEnergy(G4double inIonEnergy){ionEnergy = inIonEnergy;};
    inline void setIonMomentum(G4ThreeVector inIonMomentum) {ionMomentum = inIonMomentum;};
    inline void SetPhotonEnergy(G4double inPhotonEnergy) {photonEnergy=inPhotonEnergy;};
    inline void SetElectronMomentum(G4ThreeVector outElectronMomentum) {electronMomentum=outElectronMomentum;};
    inline void SetElectronEnergy(G4double outElectronEnergy) {electronEnergy=outElectronEnergy;};
    inline void SetHydrogenMomentum(G4ThreeVector outHydrogenMomentum) {hydrogenMomentum=outHydrogenMomentum;};
    inline void SetHydrogenEnergy(G4double outHydrogenEnergy) {hydrogenEnergy=outHydrogenEnergy;};
    inline void SetGamma(G4double inGamma) {gamma=inGamma;};
    //getters
    G4ThreeVector GetElectronMomentum(){return electronMomentum;};
    G4double GetElectronEnergy(){return electronEnergy;};
    G4ThreeVector GetHydrogenMomentum(){return hydrogenMomentum;};
    G4double GetHydrogenEnergy(){return hydrogenEnergy;};
private:
    // DO NOT CHANGE THESE NUMBERS THEY ARE FROM A FIT OF THE CROSS SECTION DATA
    const G4double fitCoefficient1 = 2.79945909e-12;
    const G4double fitCoefficient2 = 1.24982046e-8;
    const G4double fitCoefficient3 = 1.00207573e-5;
    const G4double fitCoefficient4 = 3.64205996e-3;
    const G4double fitCoefficient5 = 7.30751085e-2;

    //particle information
    G4double gamma;
    G4ThreeVector ionMomentum;
    G4double ionEnergy;
    G4ThreeVector electronMomentum;
    G4double electronEnergy;
    G4ThreeVector hydrogenMomentum;
    G4double hydrogenEnergy;
    G4double photonEnergy;
};

#endif