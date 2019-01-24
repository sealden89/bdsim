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

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4ProcessType.hh"
#include "G4Track.hh"
#include "Randomize.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSLaserPhotoDetachment::BDSLaserPhotoDetachment():
  G4VDiscreteProcess("laserphotodetachment", G4ProcessType::fUserDefined)
{;}

G4double BDSLaserPhotoDetachment::GetMeanFreePath(G4Track& track,
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

G4VParticleChange BDSLaserPhotoDetachment::PostStepDoIt(G4Track& aTrack)
{
  /*
  aParticleChange.Initialize(aTrack);


   ////////////////////////////////////////////////////////
//copied from mfp to access laser instance is clearly incorrect!

  G4ThreeVector particlePosition = aTrack.GetPolarization();
  G4double radius = std::sqrt(particlePosition[0]*particlePosition[0]+particlePosition[1]*particlePosition[1])

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

  G4double photonDensity = laser->Intensity(radius, particlePosition[2])/laser->PhotonEnergy();
  G4double crossSection = 3.2e-21*CLHEP::m2; // m^2 hard coded for now
  G4double stepTime = aTrack.GetStepLength()/aTrack.GetVelocity();//this seems bad as its not directional
//also why is G4Track.GetVelocity a G4double not vector
  G4double photoDetachmentProbability = 1.0-std::exp(-1.0*crossSection*photonDensity*stepTime);

  G4double randomNumber = G4UniformRand();

  if(photoDetachmentProbability>=randomNumber)
  {
    //create secondary electron and neutralise the other plus step out of volume with large mfp

  }

*/

}
