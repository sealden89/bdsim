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
#ifndef BDSFIELDMAGINTERPOLATED_H
#define BDSFIELDMAGINTERPOLATED_H

#include "BDSFieldMag.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

/**
 * @brief Class to provide scaling and a base class pointer for interpolator fields.
 *
 * @author Laurie Nevay
 */

class BDSFieldMagInterpolated: public BDSFieldMag
{
public:
  explicit BDSFieldMagInterpolated(G4Transform3D offset,
				   G4double      scalingIn = 1.0);

  virtual ~BDSFieldMagInterpolated(){;}

  inline G4double Scaling() const {return scaling;}
  inline void     SetScaling(G4double scalingIn) {scaling = scalingIn;}
  
private:
  G4double scaling; ///< Field value scaling.
};

#endif
