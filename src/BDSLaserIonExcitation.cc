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
#include "BDSLaserIonExcitation.hh"
#include "BDSLogicalVolumeLaser.hh"
#include "BDSLaser.hh"
#include "BDSIonExcitationEngine.hh"
#include "BDSStep.hh"
#include "BDSGlobalConstants.hh"
#include "BDSUserTrackInformation.hh"
#include "BDSElectronQuantumLevel.hh"

#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4Electron.hh"
#include "G4DecayProducts.hh"
#include "G4Hydrogen.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessType.hh"
#include "G4Proton.hh"
#include "G4step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"
#include "Randomize.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "BDSElectronOccupancy.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSLaserIonExcitation::BDSLaserIonExcitation(const G4String& processName):
  G4VDiscreteProcess(processName, G4ProcessType::fUserDefined),
  auxNavigator(new BDSAuxiliaryNavigator())
{;}

BDSLaserIonExcitation::~BDSLaserIonExcitation()
{
  delete auxNavigator;
}

G4double BDSLaserIonExcitation::GetMeanFreePath(const G4Track& track,
						G4double /*previousStepSize*/,
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

  const BDSLaser* laser = lvv->Laser();
  aParticleChange.Initialize(track);

  *forceCondition = Forced;
  return laser->Sigma0()/10;
}

G4VParticleChange* BDSLaserIonExcitation::PostStepDoIt(const G4Track& track,
						       const G4Step&  step)
{
  // get coordinates for photon desity calculations
  aParticleChange.Initialize(track);
  //copied from mfp to access laser instance is clearly incorrect!
  
  G4LogicalVolume* lv = track.GetVolume()->GetLogicalVolume();
  if (!lv->IsExtended())// not extended so can't be a laser logical volume
    {return pParticleChange;}
  
  BDSLogicalVolumeLaser* lvv = dynamic_cast<BDSLogicalVolumeLaser *>(lv);
  if (!lvv) // it's an extended volume but not ours (could be a crystal)
    {return pParticleChange;}
  // else proceed
  
  const BDSLaser* laser = lvv->Laser();
  G4double stepLength = step.GetStepLength();
  G4DynamicParticle* ion = const_cast<G4DynamicParticle*>(track.GetDynamicParticle());
  
  G4ThreeVector particlePositionGlobal = track.GetPosition();
  G4ThreeVector particleDirectionMomentumGlobal = track.GetMomentumDirection();
  const G4RotationMatrix* rot = track.GetTouchable()->GetRotation();
  const G4AffineTransform transform = track.GetTouchable()->GetHistory()->GetTopTransform();
  G4ThreeVector particlePositionLocal = transform.TransformPoint(particlePositionGlobal);
  G4ThreeVector particleDirectionMomentumLocal = transform.TransformPoint(particleDirectionMomentumGlobal).unit();

  BDSIonExcitationEngine* ionExcitationEngine = new BDSIonExcitationEngine();
  // create photon
  // photon vector in laser frame coordinates
  G4ThreeVector photonUnit(0, 0, 1);
  //translate into lab frame coordinates
  G4double photonE = (CLHEP::h_Planck * CLHEP::c_light) / laser->Wavelength();
  G4ThreeVector photonVector = photonUnit * photonE;
  photonVector.transform(*rot);
  G4LorentzVector photonLorentz = G4LorentzVector(photonVector, photonE);
  
  G4double ionEnergy = ion->GetTotalEnergy();
  G4ThreeVector ionMomentum = ion->GetMomentum();
  G4ThreeVector ionBeta = ionMomentum / ionEnergy;
  G4double ionVelocity = ionBeta.mag() * CLHEP::c_light;
  photonLorentz.boost(-ionBeta);
  G4double photonEnergy = photonLorentz.e();
  G4double crossSection = ionExcitationEngine->CrossSection(photonEnergy) * CLHEP::m2;

  ////////////////// stuff I added to check what its doing
  /*
  G4double b2 = ionBeta.mag2();
  G4double ggamma = 1.0/std::sqrt(1.0-b2);
  G4double bp = ionBeta[0]*photonVector[0]+ionBeta[1]*photonVector[1]+ionBeta[2]*photonVector[2];
  G4double tp = ggamma*(photonE+bp);
  G4double beta = std::sqrt(b2);
  G4double gamma = 96.3;
  G4double equivalentBP = photonE*beta*std::cos((6./180)*CLHEP::pi);
  G4double Eprime= photonE*ggamma*(1.0+beta*std::cos((6./180)*CLHEP::pi));
  G4double Eprimep= photonE*gamma*(1.0+beta*std::cos((6./180)*CLHEP::pi));
   */

  G4double photonFlux = laser->Intensity(particlePositionLocal, 0) / photonEnergy;
  G4LorentzVector ion4Vector = ion->Get4Momentum();
  ion4Vector.boost(-ionBeta);

  G4double ionTime = (stepLength / ionVelocity);
  G4double excitationProbability = 1.0 - std::exp(-crossSection * photonFlux * ionTime);
  const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  G4double scaleFactor = g->ScaleFactorLaser();
  G4double randomNumber = G4UniformRand();
  
  if ((excitationProbability * scaleFactor) > randomNumber)
    {

      BDSUserTrackInformation* userInfo = dynamic_cast<BDSUserTrackInformation*>(track.GetUserInformation());
      userInfo->GetElectronOccupancy()->RemoveElectrons(2,0,1);
      userInfo->GetElectronOccupancy()->AddElectrons(2,1,1);
      userInfo->GetElectronOccupancy()->SetTimeOfExciation(ion->GetProperTime(),2,1,(1/2));
      // Kinematics
      ionExcitationEngine->setIncomingGamma(photonLorentz);
      ionExcitationEngine->setIncomingIon(ion4Vector);
      ionExcitationEngine->PhotonAbsorption(ionBeta);
      G4LorentzVector scatteredIon = ionExcitationEngine->GetScatteredIonAbsorption();
      
      G4LorentzVector IonLorentz = G4LorentzVector(scatteredIon.vect().unit(),scatteredIon.e());
      
      aParticleChange.ProposeEnergy(scatteredIon.e());
      aParticleChange.ProposeMomentumDirection(IonLorentz.getX(),IonLorentz.getY(),IonLorentz.getZ());
      aParticleChange.ProposeWeight(scaleFactor);


    }
  
  return G4VDiscreteProcess::PostStepDoIt(track,step);
}
