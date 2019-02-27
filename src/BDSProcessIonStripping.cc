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

// Cross section calculation from: I Yu Tolstikhina, V P Shevelko, Physics - Uspekhi 61 (3) 247 - 279 (2018)

#include "BDSDebug.hh"
#include "BDSProcessIonStripping.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AutoDelete.hh"
#include "G4VDiscreteProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4Version.hh"

#include "G4DynamicParticle.hh"
#include "G4MaterialCutsCouple.hh"


BDSProcessIonStripping::BDSProcessIonStripping(const G4String& name, G4ProcessType aType):
G4VDiscreteProcess(name, aType)
{;}

G4bool BDSProcessIonStripping::IsApplicable(const G4ParticleDefinition& p)
{
    return (p.GetPDGCharge() != 0.0 && !p.IsShortLived() &&
            p.GetParticleType() == "nucleus");
}

G4double BDSProcessIonStripping::GetMeanFreePath(
        const G4Track& track,
        G4double   previousStepSize,
        G4ForceCondition* condition)
{
    const G4MaterialCutsCouple* couple = track.GetMaterialCutsCouple();

    G4cout << "Material " << couple->GetMaterial()->GetName() << G4endl; //DEBUG

    G4double interactionLength = DBL_MAX;

    if (track.GetDynamicParticle()->GetTotalOccupancy() > 0)
    {
        ///G4double crossSection = ComputeCrossSection(track, couple->GetMaterial())
        interactionLength = 1. / ComputeCrossSection(track, couple->GetMaterial());
    }

    G4cout << "Interaction length: " << interactionLength/CLHEP::mm << " mm" << G4endl; //DEBUG
    G4cout << "------------------------" << G4endl; //DEBUG
    return interactionLength;
}

G4double BDSProcessIonStripping::ComputeCrossSection(const G4Track& aTrack, const G4Material* aMaterial)
{
    const G4DynamicParticle *ionIn = aTrack.GetDynamicParticle();
    const G4ElectronOccupancy* electron_occupancy = ionIn->GetElectronOccupancy();


    // We want stripping cross section so look at a frame of reference where the
    // incoming ion is the target and the material atoms are the projectiles
    G4double projectile_energy = ionIn->GetTotalEnergy();

    G4double projectile_Z = ionIn->GetDefinition()->GetAtomicNumber();
    G4double projectile_charge = ionIn->GetCharge();
    G4double projectile_mass = ionIn->GetMass();

    // Obtain the dynamic qunatities needed
    // Get the number of electrons and the orbit number
    G4int orbitNumber = 0;
    G4int orbitOccupancy = 0;

    G4cout << "Total electron occupancy: " << electron_occupancy->GetTotalOccupancy() << G4endl;

    for (int i = (int)electron_occupancy->GetSizeOfOrbit(); i>=0; i--)
    {
        if (electron_occupancy->GetOccupancy(i))
        {
            orbitNumber= i;
            orbitOccupancy = electron_occupancy->GetOccupancy(i);

            G4cout << "Electron occupancy in shell "<< orbitNumber << " : " << orbitOccupancy << G4endl;
            //break; //Only look in the outermost orbit
        }
    }

    const G4ElementVector* theElementVector = aMaterial->GetElementVector();
    const G4double* NbOfAtomsPerVolume = aMaterial->GetVecNbOfAtomsPerVolume();

    ///G4cout << "Charge of ion : " << projectile_charge << G4endl; //DEBUG
    ///G4cout << "Mass of ion : " << projectile_mass/CLHEP::MeV << G4endl; //DEBUG
    ///G4cout << "Energy of ion : " << projectile_energy/CLHEP::GeV << G4endl; //DEBUG
    ///G4cout << "Number of electrons : " << electron_occupancy->GetTotalOccupancy() << G4endl; //DEBUG
    ///G4cout << "Orbit number :"  << orbitNumber << G4endl; //DEBUG

    G4double SIGMA = 0.0;
    for ( size_t i=0 ; i < aMaterial->GetNumberOfElements() ; ++i )
    {
        G4double target_Z = (*theElementVector)[i]->GetZ();

        ///G4cout << "Number density [1/cm3]: "<< NbOfAtomsPerVolume[i]*CLHEP::cm3 <<G4endl; //DEBUG
        SIGMA += NbOfAtomsPerVolume[i] * ComputeCrossSectionPerAtom(projectile_energy,
                                                                    projectile_mass,
                                                                    orbitNumber,
                                                                    orbitOccupancy,
                                                                    target_Z,
                                                                    projectile_Z,
                                                                    projectile_charge);
    }

    return SIGMA;
}

G4double BDSProcessIonStripping::ComputeCrossSectionPerAtom(G4double projectile_energy,
                                                            G4double projectile_mass,
                                                            G4double orbitNumber,
                                                            G4double orbitOccupancy,
                                                            G4double target_Z,
                                                            G4double projectile_Z,
                                                            G4double projectile_charge)
{
    //Constants
    G4double unit_Ry = 1;//13.6*CLHEP::eV;
    G4double c_light_au = 137; //Speed of light in atomic units. Equal to 1/alpha (fine structure constant)

    //Dynamic quantities
    G4double gamma = projectile_energy / projectile_mass;
    G4double beta = std::sqrt(1. - 1./std::pow(gamma, 2));
    G4double v = beta*c_light_au;
    G4double orbit_n = orbitNumber + 1; // The orbitNumber in geant4 is zero-counted, whereas the n quantum number counts from 1

    // Ionisation potential of target, the 2 in the denominator comes from E0=2Ry
    G4double I_nl = unit_Ry*std::pow(projectile_Z,2);//(2.*std::pow(orbitNumber+1,2)); //[APPROXIMATION], only for hydrogen-like ions
    G4double u = std::pow(v, 2) / (I_nl/unit_Ry); // Reduced energy

    ///G4cout << "===================================" << G4endl; //DEBUG
    ///G4cout << "ion mass : " << projectile_mass/CLHEP::GeV << G4endl; //DEBUG
    ///G4cout << "ion tot energy : " << projectile_energy/CLHEP::GeV << G4endl; //DEBUG
    ///G4cout << "beta : " << beta << G4endl; //DEBUG
    ///G4cout << "gamma : " << gamma << G4endl; //DEBUG
    ///G4cout << "u : " << u << G4endl; //DEBUG
    ///G4cout << "q : " << projectile_charge << G4endl; //DEBUG
    ///G4cout << "Inl : " << I_nl << G4endl; //DEBUG
    ///G4cout << "Ry : "<< unit_Ry << G4endl; //DEBUG
    ///G4cout << "Z target : " << target_Z << G4endl; //DEBUG
    ///G4cout << "Orbit n : " << orbit_n << G4endl; //DEBUG


    G4double xSectionPerAtom = 0.88E-16 * std::pow(target_Z + 1, 2) * (u / (u * u + 3.5))
                               * std::pow(unit_Ry / I_nl, 1 + 0.01 * projectile_charge)
                               * (4. + (1.31 / orbit_n) * std::log(4 * u + 1.))*CLHEP::cm2;
                                   //cross_section in cm^2/atom


    ///G4cout << "sig [cm^2/atom] = " << xSectionPerAtom/CLHEP::cm2 << G4endl; //DEBUG

    return xSectionPerAtom;
}

G4VParticleChange* BDSProcessIonStripping::PostStepDoIt(
        const G4Track& aTrack,
        const G4Step& aStep
)
{
    aParticleChange.Initialize(aTrack);

    const G4DynamicParticle *ionIn = aTrack.GetDynamicParticle();

    G4DynamicParticle* ionOut = new G4DynamicParticle(*ionIn);

    const G4ElectronOccupancy* electron_occupancy = ionOut->GetElectronOccupancy();
    for (int i = (int)electron_occupancy->GetSizeOfOrbit(); i>=0; i--)
    {
        if (electron_occupancy->GetOccupancy(i))
        {
            ionOut->RemoveElectron(i);
            break; //Only remove the first electron encountered in the outermost orbit
        }
    }

    aParticleChange.SetNumberOfSecondaries(1);
    G4double localEnergyDeposit = 0.;

    aParticleChange.AddSecondary(ionOut);
    aParticleChange.ProposeLocalEnergyDeposit(localEnergyDeposit);
    //
    // Stop the incident ion as the stripped one is emitted as a secondary
    //
    aParticleChange.ProposeMomentumDirection(0.,0.,0.);
    aParticleChange.ProposeEnergy(0.);
    aParticleChange.ProposeTrackStatus(fStopAndKill);


    return G4VDiscreteProcess::PostStepDoIt( aTrack, aStep );
}
