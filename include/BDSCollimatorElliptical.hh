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
#ifndef BDSCOLLIMATORELLIPTICAL_H
#define BDSCOLLIMATORELLIPTICAL_H

#include "BDSCollimator.hh"

#include "globals.hh" // geant4 globals / types

class G4Colour;

/**
 * @brief A class for an elliptical collimator.
 */

class BDSCollimatorElliptical: public BDSCollimator
{
public:
  BDSCollimatorElliptical(G4String  name,
			  G4double  length,
			  G4double  horizontalWidth,
			  G4double  xApertureIn        = 0,
			  G4double  yApertureIn        = 0,
			  G4double  xOutApertureIn     = 0,
			  G4double  yOutApertureIn     = 0,
			  G4String  collimatorMaterial = "copper",
			  G4String  vacuumMaterial     = "vacuum",
			  G4Colour* colour             = nullptr);
  virtual ~BDSCollimatorElliptical(){;};

  virtual void BuildInnerCollimator();

private:
  /// Private default constructor to force the use of the supplied one.
  BDSCollimatorElliptical();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCollimatorElliptical& operator=(const BDSCollimatorElliptical&) = delete;
  BDSCollimatorElliptical(BDSCollimatorElliptical&) = delete;
  ///@}
};

#endif
