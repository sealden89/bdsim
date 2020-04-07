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
#ifndef BDSHISTBINMAPPER3D_H
#define BDSHISTBINMAPPER3D_H

#include "globals.hh"
#include "G4ThreeVector.hh"

/**
 * @brief Class to map 3D histogram bin indices to a 1d vector index.
 * 
 * @author Laurie Nevay
 */

class BDSHistBinMapper3D
{
public:
  BDSHistBinMapper3D(G4int nBinsIIn,
		     G4int nBinsJIn,
		     G4int nBinsKIn);

  virtual ~BDSHistBinMapper3D(){;}

  virtual G4int GlobalFromIJKIndex(G4int iIndex,
				   G4int jIndex,
				   G4int kIndex) const;
  
  virtual void IJKFromGlobal(G4int  globalBin,
			     G4int& iIndex,
			     G4int& jIndex,
			     G4int& kIndex) const;

  inline G4double NBinsI() const {return nBinsI;}
  inline G4double NBinsJ() const {return nBinsJ;}
  inline G4double NBinsK() const {return nBinsK;}
  inline G4ThreeVector NBins() const {return G4ThreeVector(nBinsI, nBinsJ, nBinsK);}
  
private:
  G4int nBinsI;
  G4int nBinsJ;
  G4int nBinsK;
};

#endif
