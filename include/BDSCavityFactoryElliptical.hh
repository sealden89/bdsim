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
#ifndef BDSCAVITYFACTORYELLIPTICAL_H
#define BDSCAVITYFACTORYELLIPTICAL_H

#include "BDSCavityFactoryBase.hh"

#include "globals.hh"
#include "G4String.hh"

class BDSCavityInfo;

/**
 * @brief Factory for elliptical SRF cavity geometry.
 *
 * @author Stuart Walker.
 */

class BDSCavityFactoryElliptical: public BDSCavityFactoryBase
{
public:
  BDSCavityFactoryElliptical();
  virtual ~BDSCavityFactoryElliptical();
  
  virtual G4double CreateSolids(const G4String&      name,
				G4double             totalChordLength,
				const BDSCavityInfo* info);

  /// Overload base class method, but use base class method with different colour.
  virtual void SetVisAttributes(const G4String& colourName);
};

#endif
