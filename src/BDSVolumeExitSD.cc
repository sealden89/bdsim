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
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSVolumeExitHit.hh"
#include "BDSVolumeExitSD.hh"

#include "globals.hh"
#include "G4SDManager.hh"
#include "G4StepPoint.hh"
#include "G4StepStatus.hh"
#include "G4Track.hh"

BDSVolumeExitSD::BDSVolumeExitSD(G4String name,
				 G4bool   worldExitIn):
  G4VSensitiveDetector("volume_exit/" + name),
  colName(name),
  HCIDve(-1),
  collection(nullptr)
{
  collectionName.insert(name);

  statusToMatch = worldExitIn ? G4StepStatus::fWorldBoundary : fGeomBoundary;
}

void BDSVolumeExitSD::Initialize(G4HCofThisEvent* HCE)
{
  collection = new BDSVolumeExitHitsCollection(GetName(), colName);
  if (HCIDve < 0)
    {HCIDve = G4SDManager::GetSDMpointer()->GetCollectionID(collection);}
  HCE->AddHitsCollection(HCIDve, collection);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Hits Collection ID: " << HCIDve << G4endl;
#endif
}

G4bool BDSVolumeExitSD::ProcessHits(G4Step* aStep,
				    G4TouchableHistory* /*th*/)
{
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();

  if (postStepPoint->GetStepStatus() == statusToMatch)
    {
      G4double totalEnergy   = postStepPoint->GetTotalEnergy();
      G4double kineticEnergy = postStepPoint->GetKineticEnergy();
      G4ThreeVector position = postStepPoint->GetPosition();
      G4double T          = postStepPoint->GetGlobalTime();
      G4Track* track      = aStep->GetTrack();
      G4int    pdgID      = track->GetDefinition()->GetPDGEncoding();
      G4int    trackID    = track->GetTrackID();
      G4int    parentID   = track->GetParentID();
      G4double weight     = track->GetWeight();
      G4int    turnsTaken = BDSGlobalConstants::Instance()->TurnsTaken();
      
      BDSVolumeExitHit* hit = new BDSVolumeExitHit(totalEnergy,
						   kineticEnergy,
						   position.x(),
						   position.y(),
						   position.z(),
						   T,
						   pdgID,
						   trackID,
						   parentID,
						   weight,
						   turnsTaken);
      collection->insert(hit);
      return true;
    }
  else
    {return false;}
}
