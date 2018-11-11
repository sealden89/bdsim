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
    G4double density = couple->GetMaterial()->GetDensity();

    //`G4cout << "Density of "<< couple->GetMaterial()->GetName()<< "  "<< density <<G4endl;
    G4double DTHRESH = 7.519708408e+15; // Density of air in internal units

    G4double interactionLength;
    if (track.GetDynamicParticle()->GetTotalOccupancy() && density > DTHRESH)
    {
        interactionLength = 1E-3;
    }
    else
    {
        interactionLength = DBL_MAX;
    }
    return interactionLength;
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
    aParticleChange.ProposeMomentumDirection( 0., 0., 0. );
    aParticleChange.ProposeEnergy( 0. );
    aParticleChange.ProposeTrackStatus( fStopAndKill );


    return G4VDiscreteProcess::PostStepDoIt( aTrack, aStep );
}

G4double BDSProcessIonStripping::GetMeanFreePath(const G4Track& aTrack,
                                 G4double   previousStepSize,
                                 G4ForceCondition* condition
)
{ return PostStepGetPhysicalInteractionLength(aTrack, previousStepSize, condition); }

