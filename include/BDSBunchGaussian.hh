/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#ifndef BDSBUNCHGAUSSIAN_H
#define BDSBUNCHGAUSSIAN_H 

#include "BDSBunch.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"

#include <vector>

namespace CLHEP
{
  class HepRandomEngine;
  class RandMultiGauss;
}

namespace GMAD
{
  class Beam;
}

/**
 * @brief Common functionality for a 6D Gaussian distribution.
 * 
 * @author Stewart Boogert
 */

class BDSBunchGaussian: public BDSBunch
{
public:
  BDSBunchGaussian();
  virtual ~BDSBunchGaussian();
  virtual void SetOptions(const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);
  
protected:
  /// Create multidimensional Gaussian random number generator
  /// for Twiss and Gauss. Can change sigma matrix to make non-definite.
  CLHEP::RandMultiGauss* CreateMultiGauss(CLHEP::HepRandomEngine& anEngine,
					  const CLHEP::HepVector& mu,
					  CLHEP::HepSymMatrix&    sigma);
  
  /// Pregenerate all the particle coordinates and subtract the sample mean.
  void PreGenerateEvents();

  CLHEP::HepVector    meansGM;
  CLHEP::HepSymMatrix sigmaGM;

  /// Randon number generator with sigma matrix and mean.
  CLHEP::RandMultiGauss* gaussMultiGen;
  
  G4bool offsetSampleMean; ///< Whether to offset the sample mean.

  /// @{ Holder for pre-calcalculated coordinates.
  std::vector<G4double> x0_v, xp_v, y0_v, yp_v, z0_v, zp_v,E_v,t_v,weight_v;
  /// @}
  G4int iPartIteration; ///< Iterator for reading out pre-calculate coordinates
};

#endif
