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
#ifndef BDSSAMPLERCYLINDER_H
#define BDSSAMPLERCYLINDER_H

#include "BDSSampler.hh"

#include "globals.hh" // geant4 types / globals

/** 
 * @brief Cylindrical sampler around an object.
 * 
 * Creates a hollow very thin (1um thick) cylinder around an object
 * without end caps that acts as a sampler.
 */

class BDSSamplerCylinder: public BDSSampler
{
public:
  BDSSamplerCylinder(G4String name,
		     G4double lengthIn,
		     G4double radiusIn);

  virtual ~BDSSamplerCylinder(){;}

private:
  /// Private default constructor to ensure use of provided one.
  BDSSamplerCylinder();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSSamplerCylinder& operator=(const BDSSamplerCylinder&) = delete;
  BDSSamplerCylinder(BDSSamplerCylinder&) = delete;
  /// @}
};

#endif
