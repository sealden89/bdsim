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
#include "BDSFieldInfo.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSInterpolatorType.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4Transform3D.hh"
#include "G4UserLimits.hh"

#include <ostream>

G4UserLimits* BDSFieldInfo::defaultUL = nullptr;

BDSFieldInfo::BDSFieldInfo():
  fieldType(BDSFieldType::none),
  brho(0),
  integratorType(BDSIntegratorType::none),
  magnetStrength(nullptr),
  provideGlobalTransform(false),
  transform(G4Transform3D()),
  magneticFieldFilePath(""),
  magneticFieldFormat(BDSFieldFormat::none),
  magneticInterpolatorType(BDSInterpolatorType::nearest3d),
  electricFieldFilePath(""),
  electricFieldFormat(BDSFieldFormat::none),
  electricInterpolatorType(BDSInterpolatorType::nearest3d),
  cacheTransforms(true),
  eScaling(1.0),
  bScaling(1.0),
  timeOffset(0),
  autoScale(false),
  stepLimit(nullptr),
  poleTipRadius(1),
  beamPipeRadius(0),
  chordStepMinimum(-1),
  tilt(0)
{;}

BDSFieldInfo::BDSFieldInfo(BDSFieldType             fieldTypeIn,
			   G4double                 brhoIn,
			   BDSIntegratorType        integratorTypeIn,
			   const BDSMagnetStrength* magnetStrengthIn,
			   G4bool                   provideGlobalTransformIn,
			   G4Transform3D            transformIn,
			   G4String                 magneticFieldFilePathIn,
			   BDSFieldFormat           magneticFieldFormatIn,
			   BDSInterpolatorType      magneticInterpolatorTypeIn,
			   G4String                 electricFieldFilePathIn,
			   BDSFieldFormat           electricFieldFormatIn,
			   BDSInterpolatorType      electricInterpolatorTypeIn,
			   G4bool                   cacheTransformsIn,
			   G4double                 eScalingIn,
			   G4double                 bScalingIn,
			   G4double                 timeOffsetIn,
			   G4bool                   autoScaleIn,
			   G4UserLimits*            stepLimitIn,
			   G4double                 poleTipRadiusIn,
			   G4double                 beamPipeRadiusIn):
  fieldType(fieldTypeIn),
  brho(brhoIn),
  integratorType(integratorTypeIn),
  magnetStrength(magnetStrengthIn),
  provideGlobalTransform(provideGlobalTransformIn),
  transform(transformIn),
  magneticFieldFilePath(magneticFieldFilePathIn),
  magneticFieldFormat(magneticFieldFormatIn),
  magneticInterpolatorType(magneticInterpolatorTypeIn),
  electricFieldFilePath(electricFieldFilePathIn),
  electricFieldFormat(electricFieldFormatIn),
  electricInterpolatorType(electricInterpolatorTypeIn),
  cacheTransforms(cacheTransformsIn),
  eScaling(eScalingIn),
  bScaling(bScalingIn),
  timeOffset(timeOffsetIn),
  autoScale(autoScaleIn),
  stepLimit(stepLimitIn),
  poleTipRadius(poleTipRadiusIn),
  beamPipeRadius(beamPipeRadiusIn),
  chordStepMinimum(-1)
{
  // back calculate tilt angle from field transform
  G4ThreeVector unitY(0,1,0);
  G4ThreeVector unitYR = unitY.transform(transformIn.getRotation());
  tilt = CLHEP::halfpi - unitYR.getPhi(); // halfpi offset for which axis we choose by convention
}

BDSFieldInfo::~BDSFieldInfo()
{
  delete magnetStrength;
  delete stepLimit;
}

BDSFieldInfo::BDSFieldInfo(const BDSFieldInfo& other):
  fieldType(other.fieldType),
  brho(other.brho),
  integratorType(other.integratorType),
  provideGlobalTransform(other.provideGlobalTransform),
  transform(other.transform),
  magneticFieldFilePath(other.magneticFieldFilePath),
  magneticFieldFormat(other.magneticFieldFormat),
  magneticInterpolatorType(other.magneticInterpolatorType),
  electricFieldFilePath(other.electricFieldFilePath),
  electricFieldFormat(other.electricFieldFormat),
  electricInterpolatorType(other.electricInterpolatorType),
  cacheTransforms(other.cacheTransforms),
  eScaling(other.eScaling),
  bScaling(other.bScaling),
  timeOffset(other.timeOffset),
  autoScale(other.autoScale),
  poleTipRadius(other.poleTipRadius),
  beamPipeRadius(other.beamPipeRadius),
  chordStepMinimum(other.chordStepMinimum),
  tilt(other.tilt)
{
  if (other.magnetStrength)
    {magnetStrength = new BDSMagnetStrength(*other.magnetStrength);}
  else
    {magnetStrength = nullptr;} // also nullptr

  if (other.stepLimit)
    {stepLimit = new G4UserLimits(*other.stepLimit);}
  else
    {stepLimit = nullptr;}
}

void BDSFieldInfo::SetUserLimits(G4UserLimits* userLimitsIn)
{
  if (stepLimit != defaultUL)
    {delete stepLimit;} // shouldn't delete global default step limit!
  stepLimit = userLimitsIn;
}

std::ostream& operator<< (std::ostream& out, BDSFieldInfo const& info)
{
  out << "Field type:        " << info.fieldType                << G4endl;
  out << "Rigidity:          " << info.brho                     << G4endl;
  out << "Integrator:        " << info.integratorType           << G4endl;
  out << "Global transform?  " << info.provideGlobalTransform   << G4endl;
  out << "B map file:        " << info.magneticFieldFilePath    << G4endl;
  out << "B map file format: " << info.magneticFieldFormat      << G4endl;
  out << "B interpolator     " << info.magneticInterpolatorType << G4endl;
  out << "E map file:        " << info.electricFieldFilePath    << G4endl;
  out << "E map file format: " << info.electricFieldFormat      << G4endl;
  out << "E interpolator     " << info.electricInterpolatorType << G4endl;
  out << "Transform caching: " << info.cacheTransforms          << G4endl;
  out << "E Scaling:         " << info.eScaling                 << G4endl;
  out << "B Scaling:         " << info.bScaling                 << G4endl;
  out << "t offset           " << info.timeOffset               << G4endl;
  out << "Auto scale         " << info.autoScale                << G4endl;
  out << "Pole tip radius:   " << info.poleTipRadius            << G4endl;
  out << "Beam pipe radius:  " << info.beamPipeRadius           << G4endl;
  out << "Chord Step Min:    " << info.chordStepMinimum         << G4endl;
  out << "Tilt:              " << info.tilt                     << G4endl;
  if (info.magnetStrength)
    {out << "Magnet strength:   " << *(info.magnetStrength)      << G4endl;}
  if (info.stepLimit)
    {
      G4Track t = G4Track(); // dummy track
      G4double maxStep = info.stepLimit->GetMaxAllowedStep(t);
      out << "Step limit:        " << maxStep << G4endl;
    }
  return out;
}

void BDSFieldInfo::Translate(G4ThreeVector translationIn)
{
  G4RotationMatrix       rm = transform.getRotation();
  G4ThreeVector translation = transform.getTranslation();
  translation += translationIn;
  transform = G4Transform3D(rm, translation);
}
