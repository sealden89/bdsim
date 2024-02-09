/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#ifndef BDSCOLLIMATORJAW_H
#define BDSCOLLIMATORJAW_H

#include "BDSCollimator.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Material.hh"

class G4Colour;
class G4VSolid;

/**
 * @brief Collimator with only two jaw and no top bit.
 *
 * @author Will Shields
 */

class BDSCollimatorJaw: public BDSCollimator
{
public:
  BDSCollimatorJaw(const G4String& nameIn,
                   G4double    lengthIn,
                   G4double    horizontalWidthIn,
                   G4double    xHalfGapIn,
                   G4double    yHalfHeightIn,
                   G4double    xsizeLeftIn,
                   G4double    xsizeRightIn,
                   G4double    leftJawTiltIn,
                   G4double    rightJawTiltIn,
                   G4bool      buildLeftJawIn,
                   G4bool      buildRightJawIn,
                   G4Material* collimatorMaterialIn,
                   G4Material* vacuumMaterialIn,
                   G4Colour*   colourIn = nullptr);
  virtual ~BDSCollimatorJaw();

  inline G4double GetJawTiltLeft() const {return jawTiltLeft;}
  inline G4double GetJawTiltRight() const {return jawTiltRight;}

protected:
  /// Check and update parameters before construction. Called at the start of Build() as
  /// we can't call a virtual function in a constructor.
  virtual void CheckParameters() override;
  
  /// Override function in BDSCollimator for totally different construction.
  virtual void Build() override;

  /// Override function in BDSCollimator for different size based container.
  virtual void BuildContainerLogicalVolume() override;

  /// To fulfill inheritance but unused.
  virtual void BuildInnerCollimator() final {;}

  G4VSolid* jawSolid;        ///< Jaw solid.
  G4double  xSizeLeft;       ///< Offset of jaw 1
  G4double  xSizeRight;      ///< Offset of jaw 2
  G4double  xHalfGap;        ///< Half gap separation between jaws.
  G4double  jawTiltLeft;     ///< Tilt of jaw 1 (angle in x-z plane)
  G4double  jawTiltRight;    ///< Tilt of jaw 2 (angle in x-z plane)
  G4double  jawHalfWidth;    ///< Half width of each jaw.
  G4double  yHalfHeight;     ///< Half height of each jaw.
  G4bool    buildLeftJaw;    ///< Build left jaw or not.
  G4bool    buildRightJaw;   ///< Build right jaw or not.
  G4bool    buildAperture;   ///< Build aperture or not.

private:
  /// Private default constructor to force the use of the supplied one.
  BDSCollimatorJaw() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCollimatorJaw& operator=(const BDSCollimatorJaw&) = delete;
  BDSCollimatorJaw(BDSCollimatorJaw&) = delete;
  /// @}
};

#endif
