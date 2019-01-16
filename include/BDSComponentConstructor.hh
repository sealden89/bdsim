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
#ifndef BDSCOMPONENTCONSTRUCTOR_H
#define BDSCOMPONENTCONSTRUCTOR_H

#include "globals.hh"

class BDSAcceleratorComponent;
namespace GMAD
{
  struct Element;
}

class BDSComponentConstructor
{
public:
  BDSComponentConstructor(){;}
  virtual ~BDSComponentConstructor(){;}

  virtual BDSAcceleratorComponent* Construct(GMAD::Element const* elementIn,
					     GMAD::Element const* prevElementIn,
					     GMAD::Element const* nextElementIn,
					     G4double currentArcLengthIn,
					     G4double brhoIn,
					     G4double beta0In) = 0;
};

#endif
