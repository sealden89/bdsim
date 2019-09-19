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
#ifndef BDSBUNCHPTC_H
#define BDSBUNCHPTC_H

#include "BDSBunch.hh"

#include "globals.hh"

#include <vector>

/**
 * @brief A bunch distribution that reads a PTC inrays file.
 * 
 * @author Stewart Boogert
 */

class BDSBunchPtc: public BDSBunch
{  
public: 
  BDSBunchPtc();
  virtual ~BDSBunchPtc(); 
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& distrType,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
			  const G4double beamlineS = 0);
  virtual BDSParticleCoordsFull GetNextParticleLocal();
  
private:
  /// Load the PTC file into memory
  void LoadPtcFile();

  /// Assign the distribution file by finding the full path of it.
  void SetDistrFile(G4String distrFileNameIn);

  G4bool   matchDistrFileLength; ///< Whether to only run the number of particles in the file.
  G4int    nRays;     ///< Number of rays in file (1 counting).
  G4String fileName;  ///< File name.
  G4int    iRay;      ///< Iterator counter for current ray.
  std::vector<double*> ptcData; ///< Data.

  G4bool loopedOver;  ///< Whether we've reset to loop over the file again.

  G4double beta = 1.0; ///< Velocity w.r.t. speed of light. Needed to convert mom. to energy.
};

#endif
