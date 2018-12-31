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
#include "BDSPhysicsIonStripping.hh"
#include "BDSProcessIonStripping.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AutoDelete.hh"
#include "G4GenericIon.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4Version.hh"

BDSPhysicsIonStripping::BDSPhysicsIonStripping():
  G4VPhysicsConstructor("BDSPhysicsIonStripping")
{;}

BDSPhysicsIonStripping::~BDSPhysicsIonStripping(){;}

void BDSPhysicsIonStripping::ConstructParticle()
{
  G4GenericIon::GenericIonDefinition();
}

void BDSPhysicsIonStripping::ConstructProcess()
{
  if (Activated())
    {return;}

    //G4ProcessType ptype = G4ProcessType(fElectromagnetic );
    BDSProcessIonStripping* stripping = new BDSProcessIonStripping("ionstripping", fElectromagnetic);
  G4AutoDelete::Register(stripping);

#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();
  while( (*aParticleIterator)() )
    {
      G4ParticleDefinition* particle = aParticleIterator->value(); 
      // add to charged particles
      if (particle == G4GenericIon::GenericIonDefinition())
	{
        //ph->RegisterProcess(stripping, particle);
        G4cout << " \n\n****************************************************************\n"
                " Ion Stripping Physics is activated. Note that the model is currently\n"
                " wrong and only does the most simplistic treatment of stripping.\n"
                " ****************************************************************" << G4endl;

        G4ProcessManager* pmanager = particle->GetProcessManager();
        pmanager->AddDiscreteProcess(stripping);
    }
    }

  SetActivated();
}


