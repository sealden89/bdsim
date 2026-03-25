/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSPhysicsScintillationBiasing.hh"

#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSWrapperScintillationBiasing.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4PhysicsListHelper.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4String.hh"
#include "G4TrackFastVector.hh"
#include "G4Types.hh"
#include "G4Version.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <set>

BDSPhysicsScintillationBiasing::BDSPhysicsScintillationBiasing(G4int reductionFactorIn):
  G4VPhysicsConstructor("BDSPhysicsScintillationReduction"),
  reductionFactor(reductionFactorIn)
{
  if (reductionFactorIn < 1)
    {throw BDSException(__METHOD_NAME__, "the reduction factor must be an integer 1 or greater.");}

}

BDSPhysicsScintillationBiasing::~BDSPhysicsScintillationBiasing()
{;}

void BDSPhysicsScintillationBiasing::ConstructParticle()
{;}

void BDSPhysicsScintillationBiasing::ConstructProcess()
{
  if (Activated())
    {return;}

#if G4VERSION_NUMBER > 1029
  auto aParticleIterator =  G4ParticleTable::GetParticleTable()->GetIterator();
#endif
  aParticleIterator->reset();
  
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
  
  while( (*aParticleIterator)() ) {
    G4ParticleDefinition* particle = aParticleIterator->value();

    G4ProcessManager* pManager = particle->GetProcessManager();
    if (!pManager)
      continue;

    G4ProcessVector* processVector = pManager->GetProcessList();

    G4VProcess* processToWrap = nullptr;

    for (G4int i=0; i < (G4int)processVector->entries(); ++i)
    {
      G4VProcess* process = (*processVector)[i];
      if (process->GetProcessName() == "Scintillation")
      {
        processToWrap = process;
        break;
      }
    }

    if (processToWrap)
    {
      auto wrappedProcess =
        new BDSWrapperScintillationBiasing(processToWrap, reductionFactor);

      pManager->RemoveProcess(processToWrap);
      ph->RegisterProcess(wrappedProcess, particle);

      G4cout << "Bias> scintillation> wrapping \""
             << processToWrap->GetProcessName()
             << "\" for particle \""
             << particle->GetParticleName()
             << "\" with reduction factor "
             << reductionFactor << G4endl;
    }
  }
  
  SetActivated();
}
