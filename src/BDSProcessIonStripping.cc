/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2018.

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

// Cross section calculation from: Igor D Kaganovich et al 2006 New J. Phys. 8 278

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

G4double BDSProcessIonStripping::PostStepGetPhysicalInteractionLength(
        const G4Track& track,
        G4double   previousStepSize,
        G4ForceCondition* condition)
{
    const G4MaterialCutsCouple* couple = track.GetMaterialCutsCouple();
    G4double number_density = couple->GetMaterial()->GetTotNbOfAtomsPerVolume();
    G4cout << "Material " << couple->GetMaterial()->GetName() << G4endl;

    G4double interactionLength = DBL_MAX;
    if (track.GetDynamicParticle()->GetTotalOccupancy() > 0)
    {
        G4double crossSection = CalculateCrossSection(track);
        interactionLength = 1. / (crossSection * number_density);
    }

    G4cout << "Interaction length: " << interactionLength / CLHEP::mm << " mm" << G4endl;
    G4cout << "------------------------" << G4endl;
    return interactionLength;
}

G4double BDSProcessIonStripping::CalculateCrossSection(const G4Track& aTrack){

    const G4DynamicParticle *ionIn = aTrack.GetDynamicParticle();
    const G4ElectronOccupancy* electron_occupancy = ionIn->GetElectronOccupancy();

    //Constants
    G4double v0 = CLHEP::c_squared/CLHEP::hbar_Planck; // [cm/s] electron velocity (c^2/hbar)
    G4double E0 = 27.7*CLHEP::eV; //[eV] atomic energy (2Ry, m_e*v_0)

    // We want stripping cross section so look at a frame of reference where the
    // incoming ion is the target and the material atoms are the projectiles
    G4double energy = ionIn->GetTotalEnergy();

    G4double Z_target = ionIn->GetDefinition()->GetAtomicNumber();

    G4double Z_projectile = 6;//aTrack.GetMaterialCutsCouple()->GetMaterial()->GetZ();
    G4double mass = 12;// * CLHEP::amu_c2;//aTrack.GetMaterialCutsCouple()->GetMaterial()->GetA();//[APPROXIMATION], really need mass in amu

    // Obtain the dynamic qunatities needed
    // Get the number of electrons and the orbit number
    G4int orbitNumber = 0;
    G4int orbitOccupancy = 0;
    for (int i = (int)electron_occupancy->GetSizeOfOrbit(); i>=0; i--)
    {
        if (electron_occupancy->GetOccupancy(i))
        {
            orbitNumber= i;
            orbitOccupancy = electron_occupancy->GetOccupancy(i);
            break; //Only look in the outermost orbit
        }
    }

    G4double v = v0*0.2*std::sqrt(energy*CLHEP::keV/mass); // E[keV], mass[amu], velocity of incoming projectile
    // Ionisation potential of target, the 2 in the denominator comes from E0=2Ry
    G4double I_nl = E0*std::pow(Z_target,2)/(2.*std::pow(orbitNumber+1,2)); //[APPROXIMATION], only for hydrogen-like ions
    G4double v_nl = v0*std::sqrt(2*I_nl/E0); // orbital velocity of electron in target

    G4double crossSection =
            CLHEP::pi*std::pow(CLHEP::Bohr_radius,2)*std::pow(Z_projectile,2)/(Z_projectile/Z_target+1)*
            orbitOccupancy*std::pow(E0,2)/std::pow(I_nl,2)*ScalingFunctionGNew(v/v_nl/std::sqrt(Z_projectile/Z_target)+1);

    // Alternative way to get v/v0
    //G4double gamma = (float)energy/mass; // relativistic gamma
    //G4double beta = std::sqrt(1-1./std::pow(gamma, 2.));
    //G4double velocity = beta*CLHEP::fine_structure_const;

    G4cout << crossSection / CLHEP::barn << G4endl;

    return crossSection;
}

G4double BDSProcessIonStripping::ScalingFunctionGNew(G4double x)
{
    return std::exp(-1/std::pow(x,2))/std::pow(x,2)*(1.26 + 0.283*std::log(2*std::pow(x,2) + 25));
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

G4double BDSProcessIonStripping::GetMeanFreePath(const G4Track& aTrack,
                                 G4double   previousStepSize,
                                 G4ForceCondition* condition
)
{ return PostStepGetPhysicalInteractionLength(aTrack, previousStepSize, condition); }

