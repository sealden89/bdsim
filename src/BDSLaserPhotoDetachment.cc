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
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4ProcessType.hh"
#include "G4Track.hh"
#include "Randomize.hh"
#include "G4Proton.hh"
#include "G4Electron.hh"
#include "G4Hydrogen.hh"
#include "BDSStep.hh"
#include "G4VTouchable.hh"
#include "G4ThreeVector.hh"
#include "BDSPhotoDetachmentEngine.hh"



#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSLaserPhotoDetachment::BDSLaserPhotoDetachment(const G4String& processName):
  G4VDiscreteProcess("laserPhotoDetachment", G4ProcessType::fUserDefined)
{
    //hydrogen = new G4DynamicParticle(G4Hydrogen::Definition(), G4ThreeVector(0,1,0),3);
    //ahydrogen = new G4Hydrogen();
}

BDSLaserPhotoDetachment::~BDSLaserPhotoDetachment()
{;}

G4double BDSLaserPhotoDetachment::GetMeanFreePath(const G4Track& track,
                                                  G4double previousStepSize,
						  G4ForceCondition* forceCondition)
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
  const G4VTouchable* laserVolume = track.GetTouchable();
  G4ThreeVector laserPosition = laserVolume->GetTranslation();
  const G4RotationMatrix* laserRotation = laserVolume->GetRotation();
  G4double localX = laserPosition[0]-particlePosition[0];
  G4double localY = laserPosition[1]-particlePosition[1];
  G4double localZ = laserPosition[2]-particlePosition[2];
  G4double radius = std::sqrt(localX*localX+localY*localY);
  const G4DynamicParticle* ion = track.GetDynamicParticle();
  G4double ionEnergy = ion->GetTotalEnergy();
  G4ThreeVector ionMomentum = ion->GetMomentum();
  G4double ionMass = ion->GetMass();
  G4double ionVz = (ionMomentum[2]/ionEnergy)*CLHEP::c_light;
  G4double ionBetaZ = ionMomentum[2]/ionEnergy;
  G4double ionGamma = ionEnergy/ionMass;

  G4double photonEnergy = laser->PhotonEnergy(ionGamma,CLHEP::halfpi,ionBetaZ);

  BDSPhotoDetachmentEngine* photoDetachmentEngine = new BDSPhotoDetachmentEngine;
  G4double crossSection = photoDetachmentEngine->CrossSection(photonEnergy);
  const G4double photonDensity = laser->Intensity(radius,localZ)/photonEnergy;  // get position and momentum in coordinate frame of solid / laser
  // calculate mean free path
    G4cout << "in mfp " << ion->GetCharge() << G4endl;
//  G4double mfp = 1.0/(crossSection*photonDensity);
    if(ion->GetCharge()==-1)
    {

        G4double mfp = 100*CLHEP::um;
        G4cout << "mfp " << mfp << G4endl;
        return mfp;
    }

    else
    {

        G4double mfp = 1.0e100*CLHEP::m;
        G4cout << "mfp " << mfp << G4endl;
        return mfp;
    }

}

G4VParticleChange* BDSLaserPhotoDetachment::PostStepDoIt(const G4Track& track ,
							const G4Step&  step)
{

    // get coordinates for photon desity calculations
  aParticleChange.Initialize(track);
  aParticleChange.SetNumberOfSecondaries(1);

  const G4DynamicParticle* ion = track.GetDynamicParticle();
  G4double ionEnergy = ion->GetTotalEnergy();
  G4ThreeVector ionMomentum = ion->GetMomentum();
  G4double ionMass = ion->GetMass();
  G4double ionVz = (ionMomentum[2]/ionEnergy)*CLHEP::c_light;
  G4double ionBetaZ = ionMomentum[2]/ionEnergy;
  G4double ionGamma = ionEnergy/ionMass;
  G4cout << "before set " << ion->GetCharge()<< G4endl;

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
  const BDSLaser* laser = lvv->Laser();

 // auto thing = track.GetVolume()->GetRotation();
  // photon density
  G4double photonEnergy = laser->PhotonEnergy(ionGamma,CLHEP::halfpi,ionBetaZ);

  BDSPhotoDetachmentEngine* photoDetachmentEngine = new BDSPhotoDetachmentEngine;

  G4DynamicParticle* hydrogen = new G4DynamicParticle(G4Hydrogen::Definition(),ionMomentum);

  G4double hydrogenMass = hydrogen->GetMass();

  aParticleChange.ProposeCharge(0);
  aParticleChange.ProposeMass(hydrogenMass);

  G4ThreeVector hydrogenMomentum;
  hydrogenMomentum.setX(std::sqrt(1-0.99*0.99));
  hydrogenMomentum.setY(0);
  hydrogenMomentum.setZ(0.99);

  aParticleChange.ProposeMomentumDirection(hydrogenMomentum);
  G4cout << "after set " << ion->GetCharge() << G4endl;
  //secondary energy and momentum calculations in engine
  photoDetachmentEngine->SetIonEnergy(ionEnergy);
  photoDetachmentEngine->SetHydrogenMomentum(ionMomentum);
  photoDetachmentEngine->SetPhotonEnergy(photonEnergy);
  photoDetachmentEngine->SetGamma(ionGamma);
  photoDetachmentEngine->PerformPhotoDetachment();
  G4double electronEnergy = 0.51264*CLHEP::MeV;
  G4LorentzVector electronMomentum;
  electronMomentum.setPx(0);
  electronMomentum.setPy(std::sqrt(1-0.99*0.99));
  electronMomentum.setPz(0.99);
  electronMomentum.setE(electronEnergy);
  G4DynamicParticle* electron = new G4DynamicParticle(G4Electron::ElectronDefinition(),electronMomentum);// electronEnergy);
  aParticleChange.AddSecondary(electron);




  return G4VDiscreteProcess::PostStepDoIt(track,step);
}

