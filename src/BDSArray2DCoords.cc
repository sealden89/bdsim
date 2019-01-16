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
#include "BDSArray2DCoords.hh"

#include <ostream>

#include "globals.hh"

BDSArray2DCoords::BDSArray2DCoords(G4int nXIn, G4int nYIn,
				   G4double xMinIn, G4double xMaxIn,
				   G4double yMinIn, G4double yMaxIn,
				   BDSDimensionType xDimensionIn,
				   BDSDimensionType yDimensionIn):
  BDSArray3DCoords(nXIn,nYIn,1,
		   xMinIn,xMaxIn,
		   yMinIn,yMaxIn,
		   0,   1,
		   xDimensionIn,
		   yDimensionIn)
{;}

std::ostream& operator<< (std::ostream& out, BDSArray2DCoords const &a)
{
  return a.Print(out);
}
