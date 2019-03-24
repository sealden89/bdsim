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

#include "BDSPhysicsLaserIonExcitation.hh"
#include "BDSLaserIonExcitation.hh"
#include "globals.hh" // geant4 types / globals
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4Positron.hh"
#include "G4ProcessManager.hh"
#include "G4Proton.hh"
#include "G4Version.hh"
#include "G4Hydrogen.hh"
#include "G4GenericIon.hh"
#include "G4AutoDelete.hh"
#include "G4IonTable.hh"
#include "G4IonTable.hh"

BDSPhysicsLaserIonExcitation::BDSPhysicsLaserIonExcitation():
        G4VPhysicsConstructor("BDSPhysicsLaserPhotoDetachment")
{;}

BDSPhysicsLaserIonExcitation::~BDSPhysicsLaserIonExcitation()
{;}

void BDSPhysicsLaserIonExcitation::ConstructParticle()
{
    G4Electron::ElectronDefinition();
    G4GenericIon::Definition();
    G4Hydrogen::Definition();

}

void BDSPhysicsLaserIonExcitation::ConstructProcess()
{
    if (Activated())
    {return;}

    BDSLaserIonExcitation* laserIonExcitation = new BDSLaserIonExcitation();
    G4AutoDelete::Register(laserIonExcitation);
#if G4VERSION_NUMBER > 1029
    auto aParticleIterator = GetParticleIterator();
#endif
    aParticleIterator->reset();
     while((*aParticleIterator)())
    {
        G4ParticleDefinition *particle = aParticleIterator->value();
        G4ThreeVector holder;
        holder.set(0,0,1);
        //G4DynamicParticle* incoming = new G4DynamicParticle(particle,holder);
        G4double atomic = particle->GetAtomicNumber();

        if(G4IonTable::IsIon(particle)&&atomic==1)
        {
            G4cout << " \n\n****************************************************************\n"
                    " Photodetachment Physics is activated. Note that the model is currently\n"
                    " wrong and has incorrect mean free path calculation.\n"
                    " ****************************************************************" << G4endl;

            G4ProcessManager *pmanager = particle->GetProcessManager();
            G4ProcessVector *v = pmanager->GetProcessList();
            G4int photoDetIndex = 0;
            for(G4int i=0;i< (G4int) v[0].size();i++)
            {
                if(v[0][i]->GetProcessName() == "IonExcite")
                {
                    photoDetIndex = pmanager->GetProcessIndex(v[0][i]);
                }
            }

            if(photoDetIndex)
            {
                pmanager->RemoveProcess(photoDetIndex);
            }
            pmanager->AddDiscreteProcess(laserIonExcitation);
        }
    }

    SetActivated();

}