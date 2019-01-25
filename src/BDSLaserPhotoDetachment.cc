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
#include "G4Electron.hh"
#include "G4ThreeVector.hh"


#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSLaserPhotoDetachment::BDSLaserPhotoDetachment():
  G4VDiscreteProcess("laserphotodetachment", G4ProcessType::fUserDefined)
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
  // get position and momentum in coordinate frame of solid / laser
  // calculate mean free path
  G4double mfp = 1*CLHEP::um; // hard coded for now
  return mfp;
}

G4VParticleChange* BDSLaserPhotoDetachment::PostStepDoIt(const G4Track& track ,
							const G4Step&  step)
{
  aParticleChange.Initialize(track);
  G4ThreeVector particlePosition = track.GetPosition();
  //G4double radius = std::sqrt(particlePosition.x()*particlePosition[0]+particlePosition[1]*particlePosition[1])

    G4cout << "Z " << particlePosition.getZ() << G4endl;
/*
  const G4DynamicParticle* ion = aTrack.GetDynamicParticle();

   ////////////////////////////////////////////////////////
//copied from mfp to access laser instance is clearly incorrect!


  G4LogicalVolume* lv = aTrack.GetVolume()->GetLogicalVolume();
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

   /////////////////////////////////////////////////////////////

  const G4double photonDensity = laser->Intensity(radius, particlePosition[2])/laser->PhotonEnergy();
  G4double crossSection = 3.2e-21*CLHEP::m2; // m^2 hard coded for now.
  G4ThreeVector ionMomentum = ion->GetMomentumDirection();
  G4double ionZMomentum = ionMomentum[2];
  G4double ionEnergy = ion->GetTotalEnergy();
  G4double ionVz = ionZMomentum/ionEnergy;
  G4double stepTime = aTrack.GetStepLength()/ionVz;
  G4double photoDetachmentProbability = 1.0-std::exp(-1.0*crossSection*photonDensity*stepTime)
  G4double randomNumber = G4UniformRand();

  if(photoDetachmentProbability>=randomNumber)
  {
    // Generate elextron
    G4DynamicParticle* aElectron = new G4DynamicParticle(G4Electron::ElectronDefinition(),);
    aParticleChange.SetNumberOfSecondaries(1);
    aParticleChange.AddSecondary(aElectron);
    // Change H^- to H^0
    ion->SetCharge(0);
  }
*/

}

