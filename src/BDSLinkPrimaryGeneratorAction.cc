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
#include "BDSBunch.hh"
#include "BDSBunchSixTrackLink.hh"
#include "BDSDebug.hh"
#include "BDSEventInfo.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSIonDefinition.hh"
#include "BDSLinkDetectorConstruction.hh"
#include "BDSLinkEventInfo.hh"
#include "BDSLinkPrimaryGeneratorAction.hh"
#include "BDSLinkRegistry.hh"
#include "BDSPrimaryVertexInformation.hh"
#include "BDSRandom.hh"

#include "G4Event.hh"
#include "G4IonTable.hh"
#include "G4ParticleGun.hh"
#include "G4Types.hh"

BDSLinkPrimaryGeneratorAction::BDSLinkPrimaryGeneratorAction(BDSBunch* bunchIn,
							     int*      currentElementIndexIn,
							     BDSLinkDetectorConstruction* constructionIn,
							     G4bool    debugIn):
  bunch(bunchIn),
  currentElementIndex(currentElementIndexIn),
  construction(constructionIn),
  debug(debugIn),
  particleGun(nullptr)
{
  particleGun = new G4ParticleGun(1); // 1-particle gun
  
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  particleGun->SetParticlePosition(G4ThreeVector());
  particleGun->SetParticleTime(0);
}

BDSLinkPrimaryGeneratorAction::~BDSLinkPrimaryGeneratorAction()
{
  delete particleGun;
}

void BDSLinkPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // always save seed state in output
  BDSLinkEventInfo* eventInfo = new BDSLinkEventInfo();
  anEvent->SetUserInformation(eventInfo);
  eventInfo->SetSeedStateAtStart(BDSRandom::GetSeedState());

  BDSParticleCoordsFull coords;
  try
    {
      coords = bunch->GetNextParticleLocal();
      auto bunchSTL = dynamic_cast<BDSBunchSixTrackLink*>(bunch);
      if (bunchSTL)
	{
	  eventInfo->externalParticleIDofPrimary = bunchSTL->CurrentExternalParticleID();
	  eventInfo->externalParentIDofPrimary   = bunchSTL->CurrentExternalParentID();
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

  BDSParticleCoordsFullGlobal cg;
  auto lr = construction->LinkRegistry();
  const G4Transform3D tr = lr->Transform(*currentElementIndex);
  if (lr->NoRotation(*currentElementIndex))
    {
      if (debug)
        {
          G4cout << "PGA: Coords before " << coords;
          G4cout << "Offset " << tr.getTranslation() << G4endl;
        }
      BDSParticleCoords cgf = coords.ApplyOffset(tr.getTranslation());
      cg = BDSParticleCoordsFullGlobal(coords, cgf);
      if (debug)
        {G4cout << "Coords after " << cg.global;}
    }
  else
    {
      cg = BDSParticleCoordsFullGlobal(coords,(BDSParticleCoords)coords.ApplyTransform(tr));
    }
  
  particleGun->SetParticleDefinition(bunch->ParticleDefinition()->ParticleDefinition());
  
  // always update the charge - ok for normal particles; fixes purposively specified ions.
  particleGun->SetParticleCharge(bunch->ParticleDefinition()->Charge());

  // check that kinetic energy is positive and finite anyway and abort if not.
  // get the mass from the beamParticle as this takes into account any electrons
  G4double EK = cg.local.totalEnergy - bunch->ParticleDefinition()->Mass();
  if (EK <= 0)
    {
      G4cout << __METHOD_NAME__ << "Event #" << anEvent->GetEventID()
	     << " - Particle kinetic energy smaller than 0! "
	     << "This will not be tracked." << G4endl;
      anEvent->SetEventAborted();
      return;
    }

  // check the coordinates are valid
  if (!worldExtent.Encompasses(cg.global))
    {
      G4cerr << __METHOD_NAME__ << "point: " << cg.global
	     << "mm lies outside the world volume with extent ("
	     << worldExtent << " - event aborted!" << G4endl << G4endl;
      anEvent->SetEventAborted();
    }

#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << coords << G4endl;
#endif

  G4ThreeVector PartMomDir(cg.global.xp,cg.global.yp,cg.global.zp);
  G4ThreeVector PartPosition(cg.global.x,cg.global.y,cg.global.z);

  particleGun->SetParticlePosition(PartPosition);
  particleGun->SetParticleEnergy(EK);
  particleGun->SetParticleMomentumDirection(PartMomDir);
  particleGun->SetParticleTime(cg.global.T);

  particleGun->GeneratePrimaryVertex(anEvent);

  // set the weight
  auto vertex = anEvent->GetPrimaryVertex();
  vertex->SetWeight(cg.local.weight);
  //vertex->Print();

  // associate full set of coordinates with vertex for writing to output after event
  //vertex->SetUserInformation(new BDSPrimaryVertexInformation(coords,
  //							     bunch->ParticleDefinition()));

#ifdef BDSDEBUG
  vertex->Print();
#endif
}
