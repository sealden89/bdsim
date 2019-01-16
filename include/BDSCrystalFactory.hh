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
#ifndef BDSCRYSTALFACTORY_H
#define BDSCRYSTALFACTORY_H

#include "BDSCrystal.hh"
#include "BDSCrystalInfo.hh"

#include "globals.hh"         // geant4 globals / types
#include "G4RotationMatrix.hh"

class BDSExtent;
class G4LogicalVolume;
class G4Material;
class G4PVPlacement;
class G4UserLimits;
class G4VSolid;

/**
 * @brief Abstract base class for crystal factory classes.
 * 
 * Class to construct required crystal geometry. Normally our factories
 * are defined by an abstract base class, but as the required geometry is
 * just one primitive, this isn't required and we can do everything in one
 * factory.
 * 
 * @author Laurie Nevay
 */

class BDSCrystalFactory
{
public:
  BDSCrystalFactory();
  virtual ~BDSCrystalFactory(){;}

  /// Main interface to create a crystal.
  BDSCrystal* CreateCrystal(const G4String&       nameIn,
			    const BDSCrystalInfo* recipe);

private:
  /// Initialiser and can be used to reset factory pointers after use.
  /// Note, non-virtual as used in constructor.
  void CleanUp();
  
  /// Common construction tasks.
  void CommonConstruction(const G4String&       nameIn,
			  const BDSCrystalInfo* recipe);
  
  /// build beampipe and register logical volumes
  BDSCrystal* BuildCrystalObject(const BDSExtent& extent);
  
  void SetVisAttributes();                    ///< Set visual attributes.  
  void SetUserLimits(const G4double& length); ///< Set user limits.

  /// Create box geometry for a crystal.
  BDSCrystal* CreateCrystalBox(const G4String&       nameIn,
			       const BDSCrystalInfo* recipe);

  /// Create cylinder geometry for a crystal.
  BDSCrystal* CreateCrystalCylinder(const G4String&       nameIn,
				    const BDSCrystalInfo* recipe);

  /// Create torus geometry for a cyrstal.
  BDSCrystal* CreateCrystalTorus(const G4String&       nameIn,
				 const BDSCrystalInfo* recipe);

  /// Calculate the start angle and sweep angle given a bending angle.
  /// Different formula if bending angle is -ve or +ve.
  void CalculateSolidAngles(const G4double& bendingAngle,
			    G4double&       startAngle,
			    G4double&       sweepAngle) const;

  /// Produce an extent for a curved crystal.
  BDSExtent CalculateExtents(const G4double& bendingAngle,
			     const G4double& xBendingRadius,
			     const G4double& xThickness,
			     const BDSCrystalInfo* recipe) const;

  /// Utility function to calculate bending radius.
  inline G4double BendingRadius(const G4double& length,
				const G4double& angle) const
  {return length / angle;}

  /// Return bending radius in horizontal.
  inline G4double BendingRadiusHorizontal(const BDSCrystalInfo* recipe) const
  {return BendingRadius(recipe->lengthZ, recipe->bendingAngleYAxis);}

  /// Return bending radius in vertical.
  inline G4double BendingRadiusVertical(const BDSCrystalInfo* recipe) const 
  {return BendingRadius(recipe->lengthY, recipe->bendingAngleZAxis);}

  
  const G4double   maxStepFactor;      ///< Fraction of length for maximum step in user limits.
  const G4double   nSegmentsPerCircle; ///< For visualisation improvement.
  const G4int      nPoints;            ///< Number of points to split torus into.
  
  G4VSolid*         crystalSolid;
  G4LogicalVolume*  crystalLV;
  G4ThreeVector     placementOffset;
  G4RotationMatrix* placementRotation;

  /// @{ For non standard parts for easy registration - ie not the specific ones above.
  std::vector<G4LogicalVolume*>   allLogicalVolumes;
  std::vector<G4VPhysicalVolume*> allPhysicalVolumes;
  std::vector<G4RotationMatrix*>  allRotationMatrices;
  std::vector<G4VSolid*>          allSolids;
  std::vector<G4UserLimits*>      allUserLimits;
  std::vector<G4VisAttributes*>   allVisAttributes;
  /// @}
};

#endif
