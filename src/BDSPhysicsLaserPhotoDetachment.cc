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

#include "BDSPhysicsLaserPhotoDetachment.hh"
#include "BDSLaserPhotoDetachment.hh"
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

BDSPhysicsLaserPhotoDetachment::BDSPhysicsLaserPhotoDetachment():
        G4VPhysicsConstructor("BDSPhysicsLaserPhotoDetachment")
{;}

BDSPhysicsLaserPhotoDetachment::~BDSPhysicsLaserPhotoDetachment()
{;}

void BDSPhysicsLaserPhotoDetachment::ConstructParticle()
{
    G4Electron::ElectronDefinition();
    G4Proton::Definition();
    G4Hydrogen::Definition();

}

void BDSPhysicsLaserPhotoDetachment::ConstructProcess()
{
#if G4VERSION_NUMBER > 1029
    auto aParticleIterator = GetParticleIterator();
#endif

    aParticleIterator->reset();

    BDSLaserPhotoDetachment* laserPhotoDetachment = new BDSLaserPhotoDetachment();

    while((*aParticleIterator)())
    {
        G4ParticleDefinition *particle = aParticleIterator->value();
        G4ProcessManager *pmanager = particle->GetProcessManager();

        //if(particle == G4GenericIon::GenericIonDefinition())
        //{
            pmanager->AddProcess(laserPhotoDetachment);
            pmanager->SetProcessOrderingToLast(laserPhotoDetachment, idxPostStep);
       // }
    }
}