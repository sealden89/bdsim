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
#include "BDSAcceleratorModel.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSBeamline.hh"
#include "BDSCollimatorSD.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSStep.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ProcessType.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TransportationProcessType.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VProcess.hh"

#include <map>
#include <vector>

BDSCollimatorSD::BDSCollimatorSD(G4String name):
  BDSSensitiveDetector("collimator/" + name),
  collimatorCollection(nullptr),
  itsCollectionName(name),
  itsHCID(-1),
  auxNavigator(new BDSAuxiliaryNavigator())
{
  collectionName.insert(name);
}

BDSCollimatorSD::~BDSCollimatorSD()
{
  delete auxNavigator;
}

void BDSCollimatorSD::Initialize(G4HCofThisEvent* HCE)
{
  // Create Collimator hits collection
  collimatorCollection = new BDSCollimatorHitsCollection(GetName(), itsCollectionName);

  // Record id for use in EventAction to save time - slow string lookup by collection name
  if (itsHCID < 0)
    {itsHCID = G4SDManager::GetSDMpointer()->GetCollectionID(collimatorCollection);}
  HCE->AddHitsCollection(itsHCID, collimatorCollection);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Hits Collection ID: " << itsHCID << G4endl;
#endif
}

G4bool BDSCollimatorSD::ProcessHits(G4Step* step, G4TouchableHistory* rOHist)
{
  std::vector<G4VHit*> hits;
  return ProcessHitsOrdered(step, rOHist, hits);
}

G4bool BDSCollimatorSD::ProcessHitsOrdered(G4Step* step,
					   G4TouchableHistory* /*rOHist*/,
					   const std::vector<G4VHit*>& hits)
{
  G4VHit* lastHit = nullptr;
  BDSEnergyCounterHit* lastHitEDep = nullptr;
  if (!hits.empty())
    {
      lastHit = hits.back();
      lastHitEDep = dynamic_cast<BDSEnergyCounterHit*>(lastHit);
    }

  const G4VProcess* postProcess = step->GetPostStepPoint()->GetProcessDefinedStep();
  if (!postProcess)
    {return false;} // shouldn't happen - but for safety
  G4int  processType       = postProcess->GetProcessType();
  G4int  processSubType    = postProcess->GetProcessSubType();
  G4bool initialised       = processType != -1;
  G4bool notTransportation = processType != fTransportation;
  G4bool notGeneral        = (processType != fGeneral) && (processSubType != STEP_LIMITER);
  G4bool notParallel       = processType != fParallel;
  G4bool scatteringPoint   = initialised && notTransportation && notGeneral && notParallel;
  if (!scatteringPoint)
    {return false;} // don't store it - could just be step through thin collimator

  // get pre step point in local coordinates.
  BDSStep stepLocal = auxNavigator->ConvertToLocal(step);
  G4StepPoint* preStepPoint = step->GetPreStepPoint();
  G4ThreeVector preStepPosLocal = stepLocal.PreStepPoint();
  G4ThreeVector momGlobal       = preStepPoint->GetMomentumDirection();
  // uses existing transform cached in aux navigator here
  G4ThreeVector preStepMomLocal = auxNavigator->ConvertAxisToLocal(momGlobal);

  G4double totalEnergy = preStepPoint->GetTotalEnergy();

  // get which beam line it's in and the index
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(stepLocal.VolumeForTransform());

  // map the index to a collimator index
  BDSBeamline* clBeamline = nullptr; // curvilinar beam line
  BDSBeamline* mwBeamline = nullptr; // mass world beam line
  G4int beamlineIndex   = -1;
  G4int collimatorIndex = -1;

  if (theInfo)
    {
      beamlineIndex = theInfo->GetBeamlineIndex() - 1; // cl beam lines always have 1 more start and finish element
      clBeamline    = theInfo->GetBeamline();
      mwBeamline    = BDSAcceleratorModel::Instance()->CorrespondingMassWorldBeamline(clBeamline);
  
      auto beamlineMap = mapping.find(mwBeamline);
      if (beamlineMap != mapping.end())
	{
	  const auto& indexMap = beamlineMap->second;
	  collimatorIndex = indexMap.at(beamlineIndex);
	}
      else
	{// create map for this beam line
	  std::map<G4int, G4int> indexMap;
	  std::vector<G4int> collimatorIndices = mwBeamline->GetIndicesOfCollimators();
	  for (G4int i = 0; i < (G4int)collimatorIndices.size(); i++)
	    {indexMap[collimatorIndices[i]] = i;}
	  mapping[mwBeamline] = indexMap;
	  collimatorIndex = indexMap.at(beamlineIndex); // cl beam line
	}
    }
  
  BDSCollimatorHit* hit = new BDSCollimatorHit(mwBeamline,
					       collimatorIndex,
					       preStepPosLocal,
					       preStepMomLocal,
					       totalEnergy,
					       lastHitEDep);

  collimatorCollection->insert(hit);
  return true;
}


G4VHit* BDSCollimatorSD::last() const
{
  auto hitsVector = collimatorCollection->GetVector();
  if (hitsVector->empty())
    {return nullptr;}
  else
    {
      BDSCollimatorHit* lastHit = hitsVector->back();
      return dynamic_cast<G4VHit*>(lastHit);
    }
}
