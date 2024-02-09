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
#ifndef BDSINTEGRATOROCTUPOLE_H
#define BDSINTEGRATOROCTUPOLE_H

#include "BDSIntegratorEulerOld.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/** 
 * @brief Integrator for octupole field.
 */

class BDSIntegratorOctupole: public BDSIntegratorEulerOld
{
public:
  BDSIntegratorOctupole(BDSMagnetStrength const* strength,
			G4double                 brho,
			G4Mag_EqRhs*             eqOfMIn);
  
  virtual ~BDSIntegratorOctupole(){;}

protected:
  /// Calculate the new particle coordinates.
  virtual void AdvanceHelix(const G4double yIn[],
			    G4double       h,
			    G4double       yOut[],
                G4double       yErr[]);

private:
  BDSIntegratorOctupole() = delete;

  /// 3rd differential of field.
  G4double bTriplePrime;
};

#endif
