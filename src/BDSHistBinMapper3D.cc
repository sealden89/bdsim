/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSHistBinMapper3D.hh"

#include "globals.hh"

BDSHistBinMapper3D::BDSHistBinMapper3D(G4int nBinsIIn,
				       G4int nBinsJIn,
				       G4int nBinsKIn):
  nBinsI(nBinsIIn),
  nBinsJ(nBinsJIn),
  nBinsK(nBinsKIn)
{;}

G4int BDSHistBinMapper3D::GlobalFromIJKIndex(G4int iIndex,
					     G4int jIndex,
					     G4int kIndex) const
{
  return iIndex*nBinsJ*nBinsK + jIndex*nBinsK + kIndex;
}

void BDSHistBinMapper3D::IJKFromGlobal(G4int  globalBin,
				       G4int& iIndex,
				       G4int& jIndex,
				       G4int& kIndex) const
{
  iIndex = globalBin / (nBinsK*nBinsJ);
  jIndex = (globalBin - iIndex*nBinsK*nBinsJ) / nBinsK;
  kIndex = globalBin - jIndex*nBinsK - iIndex*nBinsK*nBinsJ;
}
