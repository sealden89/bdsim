
/*
 *
 *
 *
 *
 *
 *
 *
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
#include "G4ParticleTable.hh"
#include "G4VTouchable.hh"
#include "G4ThreeVector.hh"
#include "BDSPhotoDetachmentEngine.hh"
#include "G4AffineTransform.hh"
#include "G4StepPoint.hh"
#include "G4step.hh"



#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSLaserPhotoDetachment::BDSLaserPhotoDetachment(   const G4String& processName):
  G4VDiscreteProcess("laserPhotoDetachment", G4ProcessType::fUserDefined),auxNavigator(new BDSAuxiliaryNavigator())
{
    //hydrogen = new G4DynamicParticle(G4Hydrogen::Definition(), G4ThreeVector(0,1,0),3);
    //ahydrogen = new G4Hydrogen();
}

BDSLaserPhotoDetachment::~BDSLaserPhotoDetachment()
{
  delete auxNavigator;
}

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

  G4ThreeVector particlePosition = track.GetPosition();
  G4ThreeVector particleDirectionMomentum = track.GetMomentumDirection();

  aParticleChange.Initialize(track);
  BDSStep stepLocal = auxNavigator->ConvertToLocal(particlePosition,particleDirectionMomentum);
  const G4ThreeVector posafterlocal  = stepLocal.PreStepPoint();
  G4double theta = std::acos((particlePosition*posafterlocal)/(particlePosition.mag()*posafterlocal.mag()))-CLHEP::halfpi;
  G4double localX = posafterlocal.getX();
  G4double localY = posafterlocal.getY();
  G4double localZ = posafterlocal.getZ();
  G4double radius = std::sqrt(localX*localX+localY*localY);
  const G4DynamicParticle* ion = track.GetDynamicParticle();
  G4double ionEnergy = ion->GetTotalEnergy();
  G4ThreeVector ionMomentum = ion->GetMomentum();
  G4double ionMass = ion->GetMass();
  G4double ionBetaZ = ionMomentum.getZ()/ionEnergy;
  G4double ionGamma = ionEnergy/ionMass;

  G4double photonEnergy = laser->PhotonEnergy(ionGamma,theta,ionBetaZ);
  BDSPhotoDetachmentEngine* photoDetachmentEngine = new BDSPhotoDetachmentEngine;
  G4double crossSection = photoDetachmentEngine->CrossSection(photonEnergy);
  const G4double photonDensity = laser->Intensity(radius,localZ)/photonEnergy;  // get position and momentum in coordinate frame of solid / laser

    //G4double mfp1 = 1.0/(crossSection*photonDensity);
  G4double mfp = 100.0e-6;//1.0/(crossSection*photonDensity);
    if(ion->GetCharge()==-1)
    {

      G4double mfp = 100.0e-6;//1.0/(crossSection*photonDensity);
      return mfp;
    }

    else
    {
// correct this to be the edge of the logical volume

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
  G4double ionKe = ion->GetKineticEnergy();
  G4ThreeVector ionMomentum = ion->GetMomentum();
  G4double ionMass = ion->GetMass();
  G4double ionBetaZ = ionMomentum[2]/ionEnergy;
  G4double ionGamma = ionEnergy/ionMass;

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

  G4double hydrogenMass = (CLHEP::electron_mass_c2+CLHEP::proton_mass_c2);
  aParticleChange.ProposeMass(hydrogenMass);
  G4ThreeVector hydrogenMomentum;
  hydrogenMomentum.setX((ionMomentum[0]/ionMass)*hydrogenMass);
  hydrogenMomentum.setY((ionMomentum[1]/ionMass)*hydrogenMass);
  hydrogenMomentum.setZ((ionMomentum[2]/ionMass)*hydrogenMass);
  G4double hydrogenMomentumMagnitude = std::sqrt(hydrogenMomentum[0]*hydrogenMomentum[0]
                                                 + hydrogenMomentum[1]*hydrogenMomentum[1]
                                                 + hydrogenMomentum[2]*hydrogenMomentum[2]);
  //aParticleChange.ProposeEnergy(hydrogenMomentum*hydrogenMomentum+hydrogenMass*hydrogenMass);
  aParticleChange.ProposeMomentumDirection(hydrogenMomentum.unit());


  G4ThreeVector electronMomentum;
  electronMomentum.setX((ionMomentum[0]/ionMass)*CLHEP::electron_mass_c2);
  electronMomentum.setY((ionMomentum[1]/ionMass)*CLHEP::electron_mass_c2);
  electronMomentum.setZ((ionMomentum[2]/ionMass)*CLHEP::electron_mass_c2);
  //G4double electronMomentumMagnitude = std::sqrt(electronMomentum[0]*electronMomentum[0]
          //                                       + electronMomentum[1]*electronMomentum[1]
           //                                      + electronMomentum[2]*electronMomentum[2]);

  G4double electronKe = ionKe*(CLHEP::electron_mass_c2/ionMass);
  G4DynamicParticle* electron = new G4DynamicParticle(G4Electron::ElectronDefinition(),electronMomentum.unit(),electronKe);
  aParticleChange.AddSecondary(electron);
  aParticleChange.ProposeCharge(0);
  G4cout << electron->GetKineticEnergy() << G4endl;

  return G4VDiscreteProcess::PostStepDoIt(track,step);
}

