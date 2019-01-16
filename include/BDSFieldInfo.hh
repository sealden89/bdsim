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
#ifndef BDSFIELDINFO_H
#define BDSFIELDINFO_H

#include "BDSFieldFormat.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSInterpolatorType.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"
#include "G4ThreeVector.hh"

#include <ostream>

class BDSMagnetStrength;
class G4UserLimits;

/**
 * @brief All info required to build complete field of any type.
 * 
 * With an instance of this class, the field factory should be able
 * to create the desired fully functional field with all required 
 * associated Geant4 objects.
 * 
 * Owns the magnetic strength instance & cavity info instance.
 *
 * Owns G4ThreeVector for unitDirection. It's a pointer to save memory
 * on average.
 *
 * Pole tip radius is used for normalisation purposes for outer yoke fields. 
 * Beam pipe radius is used to decide whether there's a gap between the pole
 * and the beam pipe and therefore to use a normal inner field for that part.
 *
 * @author Laurie Nevay
 */

class BDSFieldInfo
{
public:
  /// Default constructor for zero field effectively
  BDSFieldInfo();
  BDSFieldInfo(BDSFieldType             fieldTypeIn,
	       G4double                 brhoIn,
	       BDSIntegratorType        integratorTypeIn,
	       const BDSMagnetStrength* magnetStrengthIn           = nullptr,
	       G4bool                   provideGlobalTransformIn   = true,
	       G4Transform3D            transformIn                = G4Transform3D(),
	       G4String                 magneticFieldFilePathIn    = "",
	       BDSFieldFormat           magneticFieldFormatIn      = BDSFieldFormat::bdsim1d,
	       BDSInterpolatorType      magneticInterpolatorTypeIn = BDSInterpolatorType::nearest3d,
	       G4String                 electricFieldFilePathIn    = "",
	       BDSFieldFormat           electricFieldFormatIn      = BDSFieldFormat::bdsim1d,
	       BDSInterpolatorType      electricInterpolatorTypeIn = BDSInterpolatorType::nearest3d,
	       G4bool                   cacheTransformsIn          = false,
	       G4double                 eScalingIn                 = 1.0,
	       G4double                 bScalingIn                 = 1.0,
	       G4double                 timeOffsetIn               = 0,
	       G4bool                   autoScaleIn                = false,
	       G4UserLimits*            stepLimitIn                = nullptr,
	       G4double                 poleTipRadiusIn            = 1,
	       G4double                 beamPipeRadiusIn           = 0);
  ~BDSFieldInfo();

  /// Copy constructor
  BDSFieldInfo(const BDSFieldInfo& other);
  /// Assignment operator not used
  BDSFieldInfo& operator=(const BDSFieldInfo&) = delete;

  /// @{ Accessor
  inline BDSFieldType        FieldType()                const {return fieldType;}
  inline G4double            BRho()                     const {return brho;}
  inline BDSIntegratorType   IntegratorType()           const {return integratorType;}
  inline G4Transform3D       Transform()                const {return transform;}
  inline const BDSMagnetStrength*  MagnetStrength()     const {return magnetStrength;}
  inline G4bool              ProvideGlobal()            const {return provideGlobalTransform;}
  inline G4String            MagneticFile()             const {return magneticFieldFilePath;}
  inline BDSFieldFormat      MagneticFormat()           const {return magneticFieldFormat;}
  inline BDSInterpolatorType MagneticInterpolatorType() const {return magneticInterpolatorType;}
  inline G4String            ElectricFile()             const {return electricFieldFilePath;}
  inline BDSFieldFormat      ElectricFormat()           const {return electricFieldFormat;}
  inline BDSInterpolatorType ElectricInterpolatorType() const {return electricInterpolatorType;}
  inline G4bool              CacheTransforms()          const {return cacheTransforms;}
  inline G4double            EScaling()                 const {return eScaling;}
  inline G4double            BScaling()                 const {return bScaling;}
  inline G4double            TimeOffset()               const {return timeOffset;}
  inline G4bool              AutoScale()                const {return autoScale;}
  inline G4UserLimits*       UserLimits()               const {return stepLimit;}
  inline G4double            PoleTipRadius()            const {return poleTipRadius;}
  inline G4double            BeamPipeRadius()           const {return beamPipeRadius;}
  inline G4double            ChordStepMinimum()         const {return chordStepMinimum;}
  inline G4double            Tilt()                     const {return tilt;}
  /// @}

  /// Set Transform - could be done afterwards once instance of this class is passed around.
  inline void SetTransform(const G4Transform3D& transformIn) {transform = transformIn;}

  inline void SetMagneticInterpolatorType(BDSInterpolatorType typeIn) {magneticInterpolatorType = typeIn;}
  inline void SetBScaling(const G4double& bScalingIn) {bScaling  = bScalingIn;}
  inline void SetAutoScale(const G4bool& autoScaleIn) {autoScale = autoScaleIn;}
  inline void SetScalingRadius(const G4double& poleTipRadiusIn) {poleTipRadius = poleTipRadiusIn;}
  inline void SetBeamPipeRadius(const G4double& beamPipeRadiusIn) {beamPipeRadius = beamPipeRadiusIn;}
  inline void SetChordStepMinimum(const G4double& chordStepMinimumIn) {chordStepMinimum = chordStepMinimumIn;}

  /// Delete and replace the user limits which this class owns (only if not default ul).
  void SetUserLimits(G4UserLimits* userLimitsIn);

  /// Translate - adds an additional translation to the transform member variable. May only
  /// be known at assembly time given parameterised geometry. Used by AWAKE Spectrometer only.
  void Translate(G4ThreeVector translationIn);

  /// Turn on or off transform caching.
  inline void CacheTransforms(const G4bool& cacheTransformsIn) {cacheTransforms = cacheTransformsIn;}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSFieldInfo const &info);

  static G4UserLimits* defaultUL; ///< Cache of default user limits
  
private:
  
  BDSFieldType             fieldType;
  G4double                 brho;
  BDSIntegratorType        integratorType;
  const BDSMagnetStrength* magnetStrength;
  G4bool                   provideGlobalTransform;
  G4Transform3D            transform;  ///< Transform w.r.t. solid field will be attached to
  G4String                 magneticFieldFilePath;
  BDSFieldFormat           magneticFieldFormat;
  BDSInterpolatorType      magneticInterpolatorType;
  G4String                 electricFieldFilePath;
  BDSFieldFormat           electricFieldFormat;
  BDSInterpolatorType      electricInterpolatorType;
  G4bool                   cacheTransforms;
  G4double                 eScaling;
  G4double                 bScaling;
  G4double                 timeOffset;
  G4bool                   autoScale;
  G4UserLimits*            stepLimit;
  G4double                 poleTipRadius;  ///< Radius at which point the field will be scaled to.
  G4double                 beamPipeRadius; ///< Optional radius of beam pipe.
  G4double                 chordStepMinimum;
  G4double                 tilt;           ///< Cache of tilt of field.

  // We need a default to pass back if none is specified.
  const static G4ThreeVector defaultUnitDirection;
};

#endif
