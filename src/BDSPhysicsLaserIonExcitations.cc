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
#include "BDSLaserIonExcitation.hh"
#include "BDSPhysicsLaserIonExcitation.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AutoDelete.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4GenericIon.hh"
#include "G4Hydrogen.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4Version.hh"


BDSPhysicsLaserIonExcitation::BDSPhysicsLaserIonExcitation():
  G4VPhysicsConstructor("BDSPhysicsLaserPhotoDetachment")
{;}

BDSPhysicsLaserIonExcitation::~BDSPhysicsLaserIonExcitation()
{;}

void BDSPhysicsLaserIonExcitation::ConstructParticle()
{
  G4Electron::ElectronDefinition();
  G4Gamma::Gamma();
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
  G4String name = laserIonExcitation->GetProcessName();
  while ((*aParticleIterator)())
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      
      if(G4IonTable::IsIon(particle))
        {
	  G4ProcessManager* pmanager = particle->GetProcessManager();
	  G4ProcessVector* v = pmanager->GetProcessList();
	  G4int photoDetIndex = 0;
	  for (G4int i = 0; i< (G4int)v[0].size(); i++)
            {
	      if(v[0][i]->GetProcessName() == name)
                {photoDetIndex = pmanager->GetProcessIndex(v[0][i]);}
            }
	  
	  if(photoDetIndex) // ie non-zero
            {pmanager->RemoveProcess(photoDetIndex);}
	  pmanager->AddDiscreteProcess(laserIonExcitation);
        }
    }
  
  SetActivated();
}
