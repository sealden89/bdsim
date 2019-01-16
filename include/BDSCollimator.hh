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
#ifndef BDSCOLLIMATOR_H
#define BDSCOLLIMATOR_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 types / globals

class G4Colour;
class G4VSolid;

/**
 * @brief Base class for collimators with common construction.
 * 
 * @author Laurie Nevay
 */

class BDSCollimator: public BDSAcceleratorComponent
{
public:
  BDSCollimator(G4String  name,
		G4double  length,
		G4double  horizontalWidth,
		G4String  type,
		G4double  xApertureIn        = 0,
		G4double  yApertureIn        = 0,
		G4double  xOutApertureIn     = 0,
		G4double  yOutApertureIn     = 0,
		G4String  collimatorMaterial = "copper",
		G4String  vacuumMaterial     = "vacuum",
		G4Colour* colourIn           = nullptr);
  virtual ~BDSCollimator();

  /// Accessor for collimator material.
  virtual G4String Material() const {return collimatorMaterial;}

protected:
  virtual void Build();

  virtual void BuildContainerLogicalVolume();

  /// Pure virtual function to be provided by derived classes.
  /// Must produce vacuumSolid and innerSolid - the inner is used
  /// to subtract from the mass and the vacuum is placed inside it all
  virtual void BuildInnerCollimator() = 0;

  ///@{ Geometrical objects:
  G4VSolid* collimatorSolid;
  G4VSolid* innerSolid;
  G4VSolid* vacuumSolid;
  ///@}
  G4double  horizontalWidth;    ///< Horizontal width.
  G4double  xAperture;          ///< Aperture at entrance in x dimension
  G4double  yAperture;          ///< Aperture at entrance in y dimension
  G4double  xOutAperture;       ///< Aperture at exit in x dimension
  G4double  yOutAperture;       ///< Aperture at exit in y dimension
  G4bool    tapered;            ///< Flag for tapered collimator
  G4String  collimatorMaterial; ///< Material
  G4String  vacuumMaterial;     ///< Vacuum material
  G4Colour* colour;             ///< Colour of collimator

private:
  /// Private default constructor to force the use of the supplied one.
  BDSCollimator() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCollimator& operator=(const BDSCollimator&) = delete;
  BDSCollimator(BDSCollimator&) = delete;
  /// @}
};

#endif
