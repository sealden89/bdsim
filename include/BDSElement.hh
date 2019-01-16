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
#ifndef BDSELEMENT_H
#define BDSELEMENT_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh"

/**
 * @brief A class for a generic piece of external geometry.
 * 
 * Allows any arbritary geometry and magnetic field map to be used
 * as an accelerator component in the beamline. Geometry and magnetic fields are imported
 * from an external file (each) and can be specified in various formats.
 */

class BDSElement: public BDSAcceleratorComponent
{
public:
  BDSElement(G4String name,
	     G4double length,
	     G4double horizontalWidthIn,
	     G4String geometry,
	     G4String fieldNameIn = "",
	     G4double angle       = 0);
  virtual ~BDSElement(){;}
   
private:
  /// Private default constructor to force the use of the supplied one.
  BDSElement() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSElement& operator=(const BDSElement&) = delete;
  BDSElement(BDSElement&) = delete;
  /// @}
  
  /// This does the full construction.  Loads the external geometry and field if there is
  /// one.
  virtual void BuildContainerLogicalVolume();

  G4double horizontalWidth;
  G4String geometryFileName;
  G4String fieldName;
};

#endif
