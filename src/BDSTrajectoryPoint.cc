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
#include "BDSTrajectoryPoint.hh"

#include "G4Allocator.hh"
#include "G4NavigationHistory.hh"
#include "G4ProcessType.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TransportationProcessType.hh"
#include "G4VProcess.hh"

#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSProcessMap.hh"

#include <ostream>

G4Allocator<BDSTrajectoryPoint> bdsTrajectoryPointAllocator;

// Don't use transform caching in the aux navigator as it's used for all over the geometry here.
BDSAuxiliaryNavigator* BDSTrajectoryPoint::auxNavigator = new BDSAuxiliaryNavigator();

BDSTrajectoryPoint::BDSTrajectoryPoint():
  G4TrajectoryPoint(G4ThreeVector())
{
  InitialiseVariables();
}

BDSTrajectoryPoint::BDSTrajectoryPoint(const G4Track* track):
  G4TrajectoryPoint(track->GetPosition())
{
  InitialiseVariables();

  // Need to store the creator process
  const G4VProcess* creatorProcess = track->GetCreatorProcess();
  if(creatorProcess)
    {
      preProcessType     = track->GetCreatorProcess()->GetProcessType();
      preProcessSubType  = track->GetCreatorProcess()->GetProcessSubType();
      postProcessType    = preProcessType;
      postProcessSubType = preProcessSubType;
    }

  preWeight    = track->GetWeight();
  postWeight   = preWeight;
  energy       = 0.0;                      // Does not loose any energy
  preEnergy    = track->GetKineticEnergy();
  postEnergy   = preEnergy;
  preMomentum  = track->GetMomentum();
  postMomentum = preMomentum;
  preGlobalTime = track->GetGlobalTime();
  postGlobalTime = preGlobalTime;

  // s position for pre and post step point
  G4VPhysicalVolume* curvilinearVol = auxNavigator->LocateGlobalPointAndSetup(track->GetPosition());
  BDSPhysicalVolumeInfo* info = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(curvilinearVol);

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Process (main|sub) (" << BDSProcessMap::Instance()->GetProcessName(postProcessType, postProcessSubType) << ")" << G4endl;
#endif
  if (info)
  {
    prePosLocal  = auxNavigator->ConvertToLocal(track->GetPosition());
    postPosLocal = auxNavigator->ConvertToLocal(track->GetPosition());

    G4double sCentre = info->GetSPos();
    preS             = sCentre + prePosLocal.z();
    postS            = sCentre + postPosLocal.z();
    beamlineIndex    = info->GetBeamlineIndex();
    turnstaken       = BDSGlobalConstants::Instance()->TurnsTaken();
  }

}

BDSTrajectoryPoint::BDSTrajectoryPoint(const G4Step* step):
  G4TrajectoryPoint(step->GetPostStepPoint()->GetPosition())
{
  InitialiseVariables();
  
  const G4StepPoint* prePoint    = step->GetPreStepPoint();
  const G4StepPoint* postPoint   = step->GetPostStepPoint();
  const G4VProcess*  preProcess  = prePoint->GetProcessDefinedStep();
  const G4VProcess*  postProcess = postPoint->GetProcessDefinedStep();

  if(preProcess)
    {
      preProcessType    = preProcess->GetProcessType();
      preProcessSubType = preProcess->GetProcessSubType();
    }
  
  if(postProcess)
    {
      postProcessType    = postProcess->GetProcessType();
      postProcessSubType = postProcess->GetProcessSubType();
    }

  preWeight      = prePoint->GetWeight();
  postWeight     = postPoint->GetWeight();
  energy         = step->GetTotalEnergyDeposit();
  preEnergy      = prePoint->GetKineticEnergy();
  postEnergy     = postPoint->GetKineticEnergy();
  preMomentum    = prePoint->GetMomentum();
  postMomentum   = postPoint->GetMomentum();
  preGlobalTime  = prePoint->GetGlobalTime();
  postGlobalTime = postPoint->GetGlobalTime();

  // s position for pre and post step point
  G4VPhysicalVolume* curvilinearVol = auxNavigator->LocateGlobalPointAndSetup(step);
  BDSPhysicalVolumeInfo* info = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(curvilinearVol);

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << BDSProcessMap::Instance()->GetProcessName(postProcessType, postProcessSubType) << G4endl;
#endif
  if (info)
  {
    prePosLocal  = auxNavigator->ConvertToLocal(prePoint->GetPosition());
    postPosLocal = auxNavigator->ConvertToLocal(postPoint->GetPosition());

    G4double sCentre = info->GetSPos();
    preS             = sCentre + prePosLocal.z();
    postS            = sCentre + postPosLocal.z();
    beamlineIndex    = info->GetBeamlineIndex();
    beamline         = info->GetBeamline();
    turnstaken       = BDSGlobalConstants::Instance()->TurnsTaken();
  }
}

BDSTrajectoryPoint::~BDSTrajectoryPoint()
{}

void BDSTrajectoryPoint::InitialiseVariables()
{
  preProcessType     = -1;
  preProcessSubType  = -1;
  postProcessType    = -1;
  postProcessSubType = -1;
  preWeight          = -1.;
  postWeight         = -1.;
  preEnergy          = -1.;
  postEnergy         = -1.;
  preMomentum        = G4ThreeVector();
  postMomentum       = G4ThreeVector();
  energy             = -1.;
  preS               = -1000;
  postS              = -1000;
  beamlineIndex      = -1;
  beamline           = nullptr;
  turnstaken         = 0;
  prePosLocal        = G4ThreeVector();
  postPosLocal       = G4ThreeVector();
}

G4bool BDSTrajectoryPoint::IsScatteringPoint()const
{
  auto processType    = GetPostProcessType();
  auto processSubType = GetPostProcessSubType();
  
  // test against things we want to exclude like tracking - these are not
  // points of scattering
  G4bool initialised       = processType != -1;
  G4bool notTransportation = processType != fTransportation;
  G4bool notGeneral        = (processType != fGeneral) && (processSubType != STEP_LIMITER);
  G4bool notParallel       = processType != fParallel;

  if (initialised && notTransportation && notGeneral && notParallel)
    {
#ifdef BDSDEBUG
      G4cout << "Interaction point found at "
	     << GetPreS()/CLHEP::m
	     << " m - "
	     << BDSProcessMap::Instance()->GetProcessName(processType, processSubType) << G4endl;
#endif
      return true;
    }
  return false;
}

std::ostream& operator<< (std::ostream& out, BDSTrajectoryPoint const &p)
{
  out << p.GetPosition();
  return out;
}

G4double BDSTrajectoryPoint::PrePosR() const
{
  return std::hypot(prePosLocal.x(), prePosLocal.y());
}

G4double BDSTrajectoryPoint::PostPosR() const
{
  return std::hypot(postPosLocal.x(), postPosLocal.y());
}
