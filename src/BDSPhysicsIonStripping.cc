/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSProcessIonIonisation.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AutoDelete.hh"
#include "G4GenericIon.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4IonTable.hh"
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
  BDSProcessIonIonisation* ionioni = new BDSProcessIonIonisation("myIon");
  G4AutoDelete::Register(stripping);
  G4AutoDelete::Register(ionioni);
  
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();
  while((*aParticleIterator)())
    {
      G4ParticleDefinition *particle = aParticleIterator->value();
      // add to charged particles
      if (G4IonTable::IsIon(particle))
	{
	  //ph->RegisterProcess(stripping, particle);
          G4cout << " \n\n****************************************************************\n"
	    " Ion Stripping Physics is activated. Note that the model is currently\n"
	    " wrong and only does the most simplistic treatment of stripping.\n"
	    " ****************************************************************" << G4endl;
	  
          G4ProcessManager* pmanager = particle->GetProcessManager();

          G4ProcessVector* v = pmanager->GetProcessList();
          G4int ion_ioni_index = 0;
	  
          for (G4int i = 0; i < (G4int) v[0].size(); ++i)
	    {
              if (v[0][i]->GetProcessName() == "ionIoni")
		{ion_ioni_index = pmanager->GetProcessIndex(v[0][i]);} 
	    }
	  
	  if (ion_ioni_index)
	    {pmanager->RemoveProcess(ion_ioni_index);}
	  
          //pmanager->AddProcess(ionioni,  -1, 2, 2); 
          pmanager->AddDiscreteProcess(stripping);
	}    
    }
  
  SetActivated();
}


