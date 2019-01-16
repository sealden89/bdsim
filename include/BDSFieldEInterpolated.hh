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
#ifndef BDSFIELDEINTERPOLATED_H
#define BDSFIELDEINTERPOLATED_H

#include "BDSFieldE.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

/**
 * @brief Class to provide scaling and a base class pointer for interpolator fields.
 *
 * @author Laurie Nevay
 */

class BDSFieldEInterpolated: public BDSFieldE
{
public:
  explicit BDSFieldEInterpolated(G4Transform3D offset,
				 G4double      eScalingIn = 1.0);

  virtual ~BDSFieldEInterpolated(){;}

  /// Accessor.
  inline G4double EScaling() const {return eScaling;}

  /// Setter.
  inline void     SetEScaling(G4double eScalingIn) {eScaling = eScalingIn;}
  
private:
  G4double eScaling; ///< E field scaling value.
};

#endif
