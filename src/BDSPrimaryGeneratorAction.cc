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
#include "BDSBunch.hh"
#include "BDSDebug.hh"
#include "BDSEventInfo.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIonDefinition.hh"
#include "BDSOutputLoader.hh"
#include "BDSParticleDefinition.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSPrimaryVertexInformation.hh"
#include "BDSPTCOneTurnMap.hh"
#include "BDSRandom.hh"

#include "CLHEP/Random/Random.h"

#include "globals.hh" // geant4 types / globals
#include "G4Event.hh"
#include "G4IonTable.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh"
#include "G4Version.hh"

BDSPrimaryGeneratorAction::BDSPrimaryGeneratorAction(BDSBunch*              bunchIn,
						     BDSParticleDefinition* beamParticleIn):
  beamParticle(beamParticleIn),
  ionDefinition(beamParticleIn->IonDefinition()),
  bunch(bunchIn),
  recreateFile(nullptr),
  eventOffset(0),
  ionPrimary(beamParticleIn->IsAnIon()),
  ionCached(false),
  particleCharge(beamParticleIn->Charge()), // always right even if ion
  oneTurnMap(nullptr)
{
  particleGun  = new G4ParticleGun(1); // 1-particle gun

  writeASCIISeedState = BDSGlobalConstants::Instance()->WriteSeedState();
  recreate            = BDSGlobalConstants::Instance()->Recreate();
  useASCIISeedState   = BDSGlobalConstants::Instance()->UseASCIISeedState();

  if (recreate)
    {
      recreateFile = new BDSOutputLoader(BDSGlobalConstants::Instance()->RecreateFileName());
      eventOffset  = BDSGlobalConstants::Instance()->StartFromEvent();
      bunch->RecreateAdvanceToEvent(eventOffset);
    }

  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  particleGun->SetParticlePosition(G4ThreeVector(0.*CLHEP::cm,0.*CLHEP::cm,0.*CLHEP::cm));
  particleGun->SetParticleTime(0);
}

BDSPrimaryGeneratorAction::~BDSPrimaryGeneratorAction()
{
  delete particleGun;
  delete recreateFile;
}

void BDSPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // load seed state if recreating.
  if (recreate)
    {
      G4cout << __METHOD_NAME__ << "setting seed state from file" << G4endl;
      BDSRandom::SetSeedState(recreateFile->SeedState(anEvent->GetEventID() + eventOffset));
    }

  // save the seed state in a file to recover potentially unrecoverable events
  if (writeASCIISeedState)
    {BDSRandom::WriteSeedState();}

  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  if (useASCIISeedState)
    {
      G4String fileName = globals->SeedStateFileName();
      BDSRandom::LoadSeedState(fileName);
    }

  // always save seed state in output
  BDSEventInfo* eventInfo = new BDSEventInfo();
  anEvent->SetUserInformation(eventInfo);
  eventInfo->SetSeedStateAtStart(BDSRandom::GetSeedState());

  G4double mass = beamParticle->Mass();

  // update particle definition if special case of an ion - can only be done here
  // do this before call the bunch as it may use particle definition in globals
  if (ionPrimary && !ionCached)
    {
      G4IonTable* ionTable = G4ParticleTable::GetParticleTable()->GetIonTable();
      G4ParticleDefinition* ionParticleDef = ionTable->GetIon(ionDefinition->Z(),
							      ionDefinition->A(),
							      ionDefinition->ExcitationEnergy());
      beamParticle->UpdateG4ParticleDefinition(ionParticleDef);
      // Note we don't need to take care of electrons here. These are automatically
      // allocated by Geant4 when it converts the primary vertex to a dynamic particle
      // (in the process of constructing a track from it) (done in G4PrimaryTransformer)
      // this relies on the charge being set correctly - Geant4 detects this isn't the same
      // as Z and adds electrons accordingly.
#if G4VERSION_NUMBER > 1049
      // in the case of ions the particle definition is only available now
      // fix the looping thresholds now it's available
      BDS::FixGeant105ThreshholdsForParticle(ionParticleDef);
#endif
      ionCached = true;
    }

  // continue generating particles until positive finite kinetic energy.
  G4int n = 0;
  BDSParticleCoordsFullGlobal coords;
  try
    {
      while (n < 100) // prevent infinite loops
	{
	  ++n;
	  coords = bunch->GetNextParticle();
	  
	  if ((coords.local.totalEnergy - mass) > 0)
	    {break;}
	}
    }
  catch (const BDSException& exception)
    {// we couldn't safely generate a particle -> abort
      // could be because of user input file
      anEvent->SetEventAborted();
      G4cout << exception.what() << G4endl;
      G4cout << "Aborting this event (#" << anEvent->GetEventID() << ")" << G4endl;
      return;
    }
  
  if (oneTurnMap)
    {
      G4bool offsetSAndOnFirstTurn = bunch->GetUseCurvilinear();
      oneTurnMap->SetInitialPrimaryCoordinates(coords, offsetSAndOnFirstTurn);
    }

  // set particle definition
  // either from input bunch file, an ion, or regular beam particle
  const BDSParticleDefinition* particleToUse = beamParticle;
  G4ParticleDefinition* particleDef = beamParticle->ParticleDefinition();
  if (bunch->ParticleCanBeDifferentFromBeam())
    {
      particleToUse = bunch->ParticleDefinition();
      if (particleToUse->IsAnIon())
	{
	  BDSIonDefinition* id = particleToUse->IonDefinition();
	  G4IonTable* ionTable = G4ParticleTable::GetParticleTable()->GetIonTable();
	  particleDef = ionTable->GetIon(id->Z(), id->A(), id->ExcitationEnergy());
	  // See note above - we don't need to take care of electrons in this definition as
      // Geant4 will handle this for us as long as A Z and the gun charge are correct
	}
      else
	{particleDef = particleToUse->ParticleDefinition();}
      // update charge as always set explicitly with particleGun
      // note for ions, this may be different from particleDef->GetPDGCharge
      particleCharge = beamParticle->Charge();
    }

  particleGun->SetParticleDefinition(particleDef);

  // always update the charge - ok for normal particles; fixes purposively specified ions.
  particleGun->SetParticleCharge(particleCharge);
  // check that kinetic energy is positive and finite anyway and abort if not.
  // get the mass from the beamParticle as this takes into account any electrons
  G4double EK = coords.local.totalEnergy - beamParticle->Mass();
  if(EK <= 0)
    {
      G4cout << __METHOD_NAME__ << "Particle kinetic energy smaller than 0! "
	     << "This will not be tracked." << G4endl;
      anEvent->SetEventAborted();
    }

  // write initial particle position and momentum
  if (writeASCIISeedState)
    {
      std::ofstream ofstr("output.primary.txt");
      ofstr << coords.local.x  << " " << coords.local.y  << " " << coords.local.z << " "
	    << coords.local.xp << " " << coords.local.yp << " " << coords.local.zp << " "
	    << coords.local.T  << " " << coords.local.totalEnergy << " " << coords.local.weight << std::endl;
      ofstr.close();
    }

  // check the coordinates are valid
  if (!worldExtent.Encompasses(coords.global))
    {
      G4cerr << __METHOD_NAME__ << "point: " << coords.global
	     << "mm lies outside the world volume with extent ("
	     << worldExtent << " - event aborted!" << G4endl << G4endl;
      anEvent->SetEventAborted();
    }

#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << coords << G4endl;
#endif

  G4ThreeVector PartMomDir(coords.global.xp,coords.global.yp,coords.global.zp);
  G4ThreeVector PartPosition(coords.global.x,coords.global.y,coords.global.z);

  particleGun->SetParticlePosition(PartPosition);
  particleGun->SetParticleEnergy(EK);
  particleGun->SetParticleMomentumDirection(PartMomDir);
  particleGun->SetParticleTime(coords.global.T);

  particleGun->GeneratePrimaryVertex(anEvent);

  // set the weight
  auto vertex = anEvent->GetPrimaryVertex();
  vertex->SetWeight(coords.local.weight);

  // associate full set of coordinates with vertex for writing to output after event
  G4int nElectrons = 0;

  if (const BDSIonDefinition* ionDef = particleToUse->IonDefinition())
    {nElectrons = ionDef->NElectrons();}
  vertex->SetUserInformation(new BDSPrimaryVertexInformation(coords,
							     particleCharge,
							     particleToUse->BRho(),
							     particleToUse->Mass(),
							     nElectrons));

#ifdef BDSDEBUG
  vertex->Print();
#endif
}
