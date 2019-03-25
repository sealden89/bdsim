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

  const G4RotationMatrix* rot = track.GetTouchable()->GetRotation();
  const G4AffineTransform transform = track.GetTouchable()->GetHistory()->GetTopTransform();
  G4ThreeVector localPosition = transform.TransformPoint(particlePosition);

  G4ThreeVector photonUnit(0,0,1);
  photonUnit.transform(*rot);
  G4double photonE = (CLHEP::h_Planck*CLHEP::c_light)/laser->Wavelength();
  G4ThreeVector photonVector = photonUnit*photonE;
  G4LorentzVector photonLorentz = G4LorentzVector(photonVector,photonE);

  const G4DynamicParticle* ion = track.GetDynamicParticle();
  G4double ionEnergy = ion->GetTotalEnergy();
  G4ThreeVector ionMomentum = ion->GetMomentum();
  G4double ionMass = ion->GetMass();
  G4ThreeVector ionBeta = ionMomentum/ionEnergy;
  ionBeta.set(ionBeta.getX(),ionBeta.getY(),ionBeta.getZ());
  G4double ionGamma = ionEnergy/ionMass;

  photonLorentz.boost(ionBeta.getX(),ionBeta.getY(),ionBeta.getZ());
  G4double photonEnergy = photonLorentz.e();
  G4double safety = BDSGlobalConstants::Instance()->LengthSafety();

  BDSPhotoDetachmentEngine* photoDetachmentEngine = new BDSPhotoDetachmentEngine();
  G4double crossSection = photoDetachmentEngine->CrossSection(photonEnergy);


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
      G4ThreeVector temporaryPosition = localPosition+i*particleDirectionMomentum;
      const G4ThreeVector laserStepLocal = transform.TransformPoint(temporaryPosition);
      G4double radius = std::sqrt(temporaryPosition.z()*temporaryPosition.z()+temporaryPosition.y()*temporaryPosition.y());
      G4double photonDensityStep = laser->Intensity(radius,
						    temporaryPosition.x())/(photonEnergy*CLHEP::e_SI);
      totalPhotonDensity = totalPhotonDensity + photonDensityStep;

      if(photonDensityStep >= maxPhotonDensity)
        {maxPhotonDensity = photonDensityStep;}
      count =count+1.0;
    }
  G4double averagePhotonDensity = (totalPhotonDensity/count)/CLHEP::m3;
  G4double mfp = 1.0/(crossSection*averagePhotonDensity)*CLHEP::m;

  if (ion->GetCharge()==-1)
    { return mfp; }
  else
    {
      mfp = 1.0e10*CLHEP::m;
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

