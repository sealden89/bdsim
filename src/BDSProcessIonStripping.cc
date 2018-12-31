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
    //G4double number_density = couple->GetMaterial()->GetTotNbOfAtomsPerVolume();
    G4cout << "Material " << couple->GetMaterial()->GetName() << G4endl;

    G4double interactionLength = DBL_MAX;

    if (track.GetDynamicParticle()->GetTotalOccupancy() > 0)
    {
        //G4double crossSection = ComputeCrossSection(track, couple->GetMaterial());
        interactionLength = 1. / ComputeCrossSection(track, couple->GetMaterial());
    }

    //G4cout << "Interaction length: " << interactionLength / CLHEP::mm << " mm" << G4endl;
    //G4cout << "------------------------" << G4endl;
    return interactionLength;
}

G4double BDSProcessIonStripping::ComputeCrossSection(const G4Track& aTrack, const G4Material* aMaterial)
{
    const G4DynamicParticle *ionIn = aTrack.GetDynamicParticle();
    const G4ElectronOccupancy* electron_occupancy = ionIn->GetElectronOccupancy();


    // We want stripping cross section so look at a frame of reference where the
    // incoming ion is the target and the material atoms are the projectiles
    G4double energy = ionIn->GetKineticEnergy();
    G4cout << "Kintetic energy [GeV]" << energy/CLHEP::GeV << G4endl;

    G4double Z_target = ionIn->GetDefinition()->GetAtomicNumber();
    G4double mass = ionIn->GetDefinition()->GetAtomicMass();
    //G4double Z_projectile = 6;//aTrack.GetMaterialCutsCouple()->GetMaterial()->GetZ();
    //G4double mass = 12;// * CLHEP::amu_c2;//aTrack.GetMaterialCutsCouple()->GetMaterial()->GetA();//[APPROXIMATION], really need mass in amu

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

    const G4ElementVector* theElementVector = aMaterial->GetElementVector();
    const G4double* NbOfAtomsPerVolume = aMaterial->GetVecNbOfAtomsPerVolume();

    G4double SIGMA = 0.0;
    for ( size_t i=0 ; i < aMaterial->GetNumberOfElements() ; ++i )
    {
        G4double Z_projectile = (*theElementVector)[i]->GetZ();
        //G4double mass = (*theElementVector)[i]->GetAtomicMassAmu();
        SIGMA += NbOfAtomsPerVolume[i] * ComputeCrossSectionPerAtom(energy,
                                                                    mass,
                                                                    orbitNumber,
                                                                    orbitOccupancy,
                                                                    Z_target,
                                                                    Z_projectile);
    }

    return SIGMA;
}

G4double BDSProcessIonStripping::ComputeCrossSectionPerAtom(G4double energy,
                                                            G4double mass,
                                                            G4double orbitNumber,
                                                            G4double orbitOccupancy,
                                                            G4double Z_target,
                                                            G4double Z_projectile)
{
    //Constants
    G4double v0 = CLHEP::fine_structure_const*CLHEP::c_light; // atomic velocity
    G4double E0 = 27.7*CLHEP::eV; //[eV] atomic energy (2Ry, m_e*v_0)

    //energy = 207*26*CLHEP::keV;
    G4double v = v0*0.2*std::sqrt(energy/CLHEP::keV/mass); // E[keV], mass[amu], velocity of incoming projectile
    // Ionisation potential of target, the 2 in the denominator comes from E0=2Ry
    G4double I_nl = E0*std::pow(Z_target,2)/2.;//(2.*std::pow(orbitNumber+1,2)); //[APPROXIMATION], only for hydrogen-like ions
    G4double v_nl = v0*std::sqrt(2*I_nl/E0); // v0*Zt orbital velocity of electron in target

    G4double xSectionPerAtom =
            CLHEP::pi*std::pow(CLHEP::Bohr_radius,2)*std::pow(Z_projectile,2)/(Z_projectile/Z_target+1)*
            orbitOccupancy*std::pow(E0,2)/std::pow(I_nl,2)*ScalingFunctionGNew(v/(v_nl*std::sqrt(Z_projectile/Z_target+1)));

    // Alternative way to get v/v0
    //G4double gamma = (float)energy/mass; // relativistic gamma
    //G4double beta = std::sqrt(1-1./std::pow(gamma, 2.));
    //G4double velocity = beta*CLHEP::fine_structure_const;

    //G4cout << "E0 [eV] " << 27.7*CLHEP::eV << G4endl;
    ////G4cout << "v/v_nl " << v/v_nl << G4endl;
    //G4cout << "v/(v_nl*sqrt(Zp+1)) " << v/(v_nl*std::sqrt(Z_projectile+1)) << G4endl;
    //G4cout << "sig*Inl^2*(Zp+1)/(Zp^2*pi)" << xSectionPerAtom*pow(I_nl,2)*(Z_projectile+1)/(std::pow(Z_projectile,2)*CLHEP::pi) << G4endl;
    ////G4cout << "E/nucleon " << energy/CLHEP::keV/mass << " [keV/amu]"<<G4endl;
    ////G4cout << "sig/Zp^2/10-16cm^2 " << xSectionPerAtom/pow(Z_projectile,2)/CLHEP::cm/CLHEP::cm/1.e-16 << G4endl;
    //G4cout << "velocity cm/s " << v0/(CLHEP::cm/CLHEP::s)/1.E8 << G4endl;
    //G4cout << "a0 cm " << CLHEP::Bohr_radius/CLHEP::cm << G4endl;
    return xSectionPerAtom;
}

G4double BDSProcessIonStripping::ScalingFunctionGNew(G4double x)
{
    G4double res = std::exp(-1./std::pow(x,2))/std::pow(x,2)*(1.26 + 0.283*std::log(2*std::pow(x,2) + 25.));
    return res;
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

