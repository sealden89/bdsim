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
#include "BDSLaserComptonScattering.hh"
#include "BDSLogicalVolumeLaser.hh"
#include "BDSLaser.hh"
#include "BDSStep.hh"
#include "BDSGlobalConstants.hh"
#include "BDSComptonEngine.hh"
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
#include "G4Gamma.hh"


#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSLaserComptonScattering::BDSLaserComptonScattering(const G4String& processName):
  G4VDiscreteProcess(processName, G4ProcessType::fElectromagnetic),
  auxNavigator(new BDSAuxiliaryNavigator()),
  comptonEngine(new BDSComptonScatteringEngine())
{;}

BDSLaserComptonScattering::~BDSLaserComptonScattering()
{
  delete auxNavigator;
}

G4double BDSLaserComptonScattering::GetMeanFreePath(const G4Track& track,
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

  // else proceed
  const BDSLaser* laser = lvv->Laser();

  *forceCondition = Forced;
  return laser->Sigma0();
}

G4VParticleChange* BDSLaserComptonScattering::PostStepDoIt(const G4Track& track,
							 const G4Step& step)
{
  // get coordinates for photon desity calculations
  aParticleChange.Initialize(track);

  G4LogicalVolume *lv = track.GetVolume()->GetLogicalVolume();
  if (!lv->IsExtended()) {// not extended so can't be a laser logical volume
      return pParticleChange;
  }
  BDSLogicalVolumeLaser *lvv = dynamic_cast<BDSLogicalVolumeLaser *>(lv);
    if (!lvv) {// it's an extended volume but not ours (could be a crystal)
      return pParticleChange;
    }
    // else proceed
  const BDSLaser* laser = lvv->Laser();


  G4double stepLength = step.GetStepLength();

  const G4DynamicParticle *electron = track.GetDynamicParticle();

  G4ThreeVector particlePositionGlobal = track.GetPosition();
  G4ThreeVector particleDirectionMomentumGlobal = track.GetMomentumDirection();
  const G4RotationMatrix* rot = track.GetTouchable()->GetRotation();
  const G4AffineTransform transform = track.GetTouchable()->GetHistory()->GetTopTransform();
  G4ThreeVector particlePositionLocal = transform.TransformPoint(particlePositionGlobal);
  G4ThreeVector particleDirectionMomentumLocal = transform.TransformPoint(particleDirectionMomentumGlobal).unit();

  G4int partID = electron->GetParticleDefinition()->GetInstanceID();
  // create photon
  G4ThreeVector photonUnit(0,0,1);
  photonUnit.transform(*rot);
  G4double photonE = (CLHEP::h_Planck*CLHEP::c_light)/laser->Wavelength();
  G4ThreeVector photonVector = photonUnit*photonE;
  G4LorentzVector photonLorentz = G4LorentzVector(photonVector,photonE);

  G4double electronEnergy = electron->GetTotalEnergy();
  G4ThreeVector electronMomentum = electron->GetMomentum();
  G4double electronMass = electron->GetMass();
  G4ThreeVector electronBeta = electronMomentum/electronEnergy;
  //G4double electronGamma = electronEnergy/electronMass;
  G4double electronVelocity = electronBeta.mag()*CLHEP::c_light;
  photonLorentz.boost(electronBeta);
  G4double photonEnergy = photonLorentz.e();
  G4LorentzVector electron4Vector = electron->Get4Momentum();
  G4double crossSection = comptonEngine->CrossSection(photonEnergy,partID)*CLHEP::m2;

  G4double photonFlux = laser->Intensity(particlePositionLocal,0)/photonEnergy;


  G4double ionTime = (stepLength/electronVelocity);
  G4double NeutralisationProbability = 1.0-std::exp(-crossSection*photonFlux*ionTime);
  const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  G4double scaleFactor = g->ScaleFactorLaser();
  G4double randomNumber = G4UniformRand();
  if((NeutralisationProbability*scaleFactor)>randomNumber)
  {
    aParticleChange.SetNumberOfSecondaries(1);
    G4double photonAngleElectronFrame = G4UniformRand()*CLHEP::twopi;
    G4double scatteredGammaEnergy = photonEnergy/(1+(photonEnergy/electronMass)*(1-std::cos(photonAngleElectronFrame)));
    G4ThreeVector photonMomentum;
    G4ThreeVector scatteredGammaMomentum;
    photonMomentum.set(photonLorentz.getX(),photonLorentz.getY(), photonLorentz.getZ());
    scatteredGammaMomentum = scatteredGammaEnergy*photonMomentum;
    G4LorentzVector scatteredGamma = G4LorentzVector(scatteredGammaMomentum.unit(),scatteredGammaEnergy);
    scatteredGamma.boost(-1.0*electronBeta);
    G4DynamicParticle* gamma = new G4DynamicParticle (G4Gamma::Gamma(),
                                                         scatteredGamma.vect().unit(),// direction
                                                         scatteredGamma.e());

    G4double scatteredElectronEnergy = electronMass + photonEnergy - scatteredGammaEnergy;
    G4ThreeVector scatteredElectronMomentum = photonMomentum-scatteredGammaMomentum;
    G4LorentzVector electronLorentz = G4LorentzVector(scatteredElectronMomentum.unit(),scatteredElectronEnergy);
    electronLorentz.boost(-1.0*electronBeta);

    aParticleChange.AddSecondary(gamma);
    aParticleChange.ProposeEnergy(electronLorentz.e());
    aParticleChange.ProposeMomentumDirection(electronLorentz.getX(),electronLorentz.getY(),electronLorentz.getZ());

    return G4VDiscreteProcess::PostStepDoIt(track, step);

  }
  else{   return G4VDiscreteProcess::PostStepDoIt(track, step); }



}

