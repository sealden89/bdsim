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
#include "BDSLaserPhotoDetachment.hh"
#include "BDSLogicalVolumeLaser.hh"
#include "BDSLaser.hh"
#include "BDSPhotoDetachmentEngine.hh"
#include "BDSStep.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4Electron.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessType.hh"
#include "G4Proton.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"
#include "Randomize.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSLaserPhotoDetachment::BDSLaserPhotoDetachment(const G4String& processName):
  G4VDiscreteProcess(processName, G4ProcessType::fUserDefined),
  auxNavigator(new BDSAuxiliaryNavigator())
{;}

BDSLaserPhotoDetachment::~BDSLaserPhotoDetachment()
{
  delete auxNavigator;
}

G4double BDSLaserPhotoDetachment::GetMeanFreePath(const G4Track& track,
                                                  G4double /*previousStepSize*/,
						  G4ForceCondition* /*forceCondition*/)
{
  G4LogicalVolume* lv = track.GetVolume()->GetLogicalVolume();
  if (!lv->IsExtended())
    {// not extended so can't be a laser logical volume
      return DBL_MAX;
    }
  BDSLogicalVolumeLaser* lvv = dynamic_cast<BDSLogicalVolumeLaser*>(lv);
  if (!lvv)
    {// it's an extended volume but not ours (could be a crystal)
      return DBL_MAX;
    }

  // else proceed
  const BDSLaser* laser = lvv->Laser();

  aParticleChange.Initialize(track);

  G4ThreeVector particlePosition = track.GetPosition();
  G4ThreeVector particleDirectionMomentum = track.GetMomentumDirection();

 /* const G4int depth = track.GetTouchable()->GetHistory()->GetDepth();
 // track.GetTouchable()->MoveUpHistory(depth);
  const G4RotationMatrix* rotation = track.GetTouchable()->GetRotation();
  //const G4AffineTransform transform = track.GetTouchable()->GetHistory()->GetTransform();
  const G4AffineTransform transform2 = track.GetTouchable()->GetHistory()->GetTopTransform();

  //G4ThreeVector localPosition = transform.TransformPoint(particlePosition);
*/

  BDSStep stepLocal = auxNavigator->ConvertToLocal(particlePosition,particleDirectionMomentum);
  const G4ThreeVector posafterlocal = stepLocal.PreStepPoint();
  // consider this angle more
  G4double theta = std::acos((particlePosition*posafterlocal)/(particlePosition.mag()*posafterlocal.mag()))-CLHEP::halfpi;
  
  const G4DynamicParticle* ion = track.GetDynamicParticle();
  G4double ionEnergy = ion->GetTotalEnergy();
  G4ThreeVector ionMomentum = ion->GetMomentum();
  G4double ionMass = ion->GetMass();
  G4double ionBetaZ = ionMomentum.getZ()/ionEnergy;
  G4double ionGamma = ionEnergy/ionMass;

  G4double safety = BDSGlobalConstants::Instance()->LengthSafety();

  G4double photonEnergy = laser->PhotonEnergy(ionGamma,theta,ionBetaZ);
  BDSPhotoDetachmentEngine* photoDetachmentEngine = new BDSPhotoDetachmentEngine;
  G4double m2 = CLHEP::m2;
  G4double crossSection = photoDetachmentEngine->CrossSection(photonEnergy);
  //const G4double photonDensity = laser->Intensity(radius,localX)/(photonEnergy*CLHEP::e_SI);  // get position and momentum in coordinate frame of solid / laser

  //G4double mfp1 = 1.0/(crossSection*photonDensity);

  auto transportMgr = G4TransportationManager::GetTransportationManager();
  auto fLinearNavigator = transportMgr->GetNavigatorForTracking();
  //G4VPhysicalVolume* selectedVol = fLinearNavigator->LocateGlobalPointAndSetup(particlePosition,&particleDirectionMomentum,true,true);
  G4double linearStepLength = fLinearNavigator->ComputeStep(particlePosition,
							    particleDirectionMomentum,
							    9.0e99,
							    safety);
  G4double stepSize = 100.0e-6;// hard coded for now will later be based on max intensity and width
  G4double count = 0;
  G4double totalPhotonDensity = 0;
  G4double maxPhotonDensity = 0;
  for (G4double i = 0; i <= linearStepLength; i = i+stepSize)
    {
      G4ThreeVector temporaryPosition = posafterlocal + i*particleDirectionMomentum;
      BDSStep laserStepLocal = auxNavigator->ConvertToLocal(temporaryPosition,particleDirectionMomentum);
      const G4ThreeVector laserPosition = laserStepLocal.PreStepPoint();
      G4double photonDensityStep = laser->Intensity(temporaryPosition.mag(),
						    temporaryPosition.x())/(photonEnergy*CLHEP::e_SI);
      totalPhotonDensity = totalPhotonDensity + photonDensityStep;

      if(photonDensityStep >= maxPhotonDensity)
        {maxPhotonDensity = photonDensityStep;}
      count =count+1.0;
    }
  G4double averagePhotonDensity = (totalPhotonDensity/count)/CLHEP::m3;
  G4double mfp = 1.0/(crossSection*averagePhotonDensity)*CLHEP::m;

  if (ion->GetCharge()==-1)
    {//G4cout << "mfp " << mfp <<" rho bar " << averagePhotonDensity <<" rho i  " << photonDensity << " r " << radius << " z " << localX << G4endl;
      return mfp;
    }
  else
    {
      mfp = 1.0e10*CLHEP::m;
      //  G4cout << "mfp " << mfp << G4endl;
      return mfp;
    }
}

G4VParticleChange* BDSLaserPhotoDetachment::PostStepDoIt(const G4Track& track,
							 const G4Step&  step)
{
  // get coordinates for photon desity calculations
  aParticleChange.Initialize(track);
  aParticleChange.SetNumberOfSecondaries(1);

  const G4DynamicParticle* ion = track.GetDynamicParticle();
  //G4double ionEnergy = ion->GetTotalEnergy();
  G4double ionKe = ion->GetKineticEnergy();
  G4ThreeVector ionMomentum = ion->GetMomentum();
  G4double ionMass = ion->GetMass();
  //G4double ionBetaZ = ionMomentum[2]/ionEnergy;
  //G4double ionGamma = ionEnergy/ionMass;

  //copied from mfp to access laser instance is clearly incorrect!

  G4LogicalVolume* lv = track.GetVolume()->GetLogicalVolume();
  if (!lv->IsExtended())
    {// not extended so can't be a laser logical volume
      return pParticleChange;
    }
  BDSLogicalVolumeLaser* lvv = dynamic_cast<BDSLogicalVolumeLaser*>(lv);
  if (!lvv)
    {// it's an extended volume but not ours (could be a crystal)
      return pParticleChange;
    }
  // else proceed
  // const BDSLaser* laser = lvv->Laser();
  
  G4double hydrogenMass = (CLHEP::electron_mass_c2+CLHEP::proton_mass_c2);
  aParticleChange.ProposeMass(hydrogenMass);
  G4ThreeVector hydrogenMomentum = (ionMomentum / ionMass) * hydrogenMass;
  //aParticleChange.ProposeEnergy(hydrogenMomentum*hydrogenMomentum+hydrogenMass*hydrogenMass);
  aParticleChange.ProposeMomentumDirection(hydrogenMomentum.unit());


  G4ThreeVector electronMomentum = (ionMomentum / ionMass)*CLHEP::electron_mass_c2;  
  G4double electronKe = ionKe*(CLHEP::electron_mass_c2/ionMass);
  G4DynamicParticle* electron = new G4DynamicParticle(G4Electron::ElectronDefinition(),electronMomentum.unit(),electronKe);
  aParticleChange.AddSecondary(electron);
  aParticleChange.ProposeCharge(0);
  
  return G4VDiscreteProcess::PostStepDoIt(track,step);
}

