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
#include "BDSBunchGaussian.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <vector>

/// helper method
namespace {
  /// check if matrix is positive definite
  bool isPositiveDefinite(CLHEP::HepSymMatrix& S)
  {
    CLHEP::HepSymMatrix temp (S); // Since diagonalize does not take a const s
                                  // we have to copy S.
    CLHEP::HepMatrix U = diagonalize ( &temp ); // S = U Sdiag U.T()
    CLHEP::HepSymMatrix D = S.similarityT(U);   // D = U.T() S U = Sdiag
    for (G4int i = 1; i <= S.num_row(); i++)
      {
	G4double s2 = D(i,i);
	if ( s2 <= 0 )
	  {return false;}
      }
    return true;
  }
}

BDSBunchGaussian::BDSBunchGaussian(): 
  BDSBunch(),
  meansGM(CLHEP::HepVector(6)),
  sigmaGM(CLHEP::HepSymMatrix(6)),
  gaussMultiGen(nullptr)
{;}

BDSBunchGaussian::~BDSBunchGaussian()
{
  delete gaussMultiGen;
}

void BDSBunchGaussian::SetOptions(const GMAD::Beam& beam,
				  G4Transform3D beamlineTransformIn)
{
  BDSBunch::SetOptions(beam, beamlineTransformIn);

  offsetSampleMean  = beam.offsetSampleMean;
  iPartIteration    = 0;
  
  meansGM[0] = X0;
  meansGM[1] = Xp0;
  meansGM[2] = Y0;
  meansGM[3] = Yp0;
  meansGM[4] = T0;
  meansGM[5] = 1;
}

CLHEP::RandMultiGauss* BDSBunchGaussian::CreateMultiGauss(CLHEP::HepRandomEngine& anEngine,
							  const CLHEP::HepVector& mu,
							  CLHEP::HepSymMatrix& sigma)
{
  /// check if sigma matrix is positive definite
  /// if not add small offset and cout warning
  
  if (!isPositiveDefinite(sigma))
    {
      G4cout << __METHOD_NAME__ << "WARNING bunch generator sigma matrix is not positive definite" << G4endl;
      G4cout << sigma << G4endl;
      G4cout << __METHOD_NAME__ << "adding a small error to zero diagonal elements" << G4endl;
      
      // very small number especially for time, since the sigma goes with the square
      G4double small_error = 1e-50;
      
      for (G4int i=0; i<6; i++)
	{
	  if (sigma[i][i]==0)
	    {sigma[i][i] += small_error;}
	}
      
      if (!isPositiveDefinite(sigma))
	{
	  G4cout << __METHOD_NAME__ << "WARNING bunch generator sigma matrix is still not positive definite" << G4endl;
	  G4cout << sigma << G4endl;
	  G4cout << __METHOD_NAME__ << "adding a small error to all elements" << G4endl;
	  for (G4int i=0; i<6; i++)
	    {
	      for (G4int j=0; j<6; j++)
		{
		  if (sigma[i][j]==0)
		    {sigma[i][j] += small_error;}
		}
	    }
	  if (!isPositiveDefinite(sigma))
	    {
	      G4cout << __METHOD_NAME__ << "ERROR bunch generator sigma matrix is still not positive definite, giving up" << G4endl;
	      G4cout << sigma << G4endl;
	      exit(1);
	    }
	}
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "mean "  << G4endl
	 << mu    << G4endl
	 << __METHOD_NAME__ << "sigma " << G4endl
	 << sigma << G4endl;
#endif
  
  return new CLHEP::RandMultiGauss(anEngine,mu,sigma); 
}

void BDSBunchGaussian::PreGenerateEvents()
{
  // generate all required primaries first
  G4double x0,xp,y0,yp,z0,zp,E,t,weight;
  G4double x0_a = 0.0, xp_a = 0.0, y0_a = 0.0, yp_a = 0.0;
  G4double z0_a = 0.0, zp_a = 0.0, E_a  = 0.0, t_a  = 0.0;

  G4int nGenerate = BDSGlobalConstants::Instance()->NGenerate();
  for (G4int iParticle = 0; iParticle < nGenerate; ++iParticle)
    {
      CLHEP::HepVector v = gaussMultiGen->fire();
      x0 = v[0] * CLHEP::m;
      xp = v[1] * CLHEP::rad;
      y0 = v[2] * CLHEP::m;
      yp = v[3] * CLHEP::rad;
      t = v[4] * CLHEP::s;
      zp = 0.0 * CLHEP::rad;
      z0 = Z0 * CLHEP::m;
      if (finiteSigmaT)
	{z0 += t * CLHEP::c_light;}
      E = E0 * CLHEP::GeV;
      if (finiteSigmaE)
	{E *= v[5];}
      
      zp = CalculateZp(xp, yp, Zp0);
      
      ApplyTransform(x0, y0, z0, xp, yp, zp);
      
      weight = 1.0;
      
      x0_a += x0;
      xp_a += xp;
      y0_a += y0;
      yp_a += yp;
      z0_a += z0;
      zp_a += zp;
      E_a  += E;
      t_a  += t;
      
      x0_v.push_back(x0);
      xp_v.push_back(xp);
      y0_v.push_back(y0);
      yp_v.push_back(yp);
      z0_v.push_back(z0);
      zp_v.push_back(zp);
      E_v.push_back(E);
      t_v.push_back(t);
      weight_v.push_back(weight);
    }

  // Compute difference between sample mean and specified means
  x0_a = (x0_a / (G4double)nGenerate) - X0*CLHEP::m;
  xp_a = (xp_a / (G4double)nGenerate) - Xp0*CLHEP::rad;
  y0_a = (y0_a / (G4double)nGenerate) - Y0*CLHEP::m;
  yp_a = (yp_a / (G4double)nGenerate) - Yp0*CLHEP::rad;
  z0_a = (z0_a / (G4double)nGenerate) - Z0*CLHEP::m;
  zp_a = (zp_a / (G4double)nGenerate) - Zp0*CLHEP::rad;
  E_a  = (E_a  / (G4double)nGenerate) - E0*CLHEP::GeV;
  t_a  = (t_a  / (G4double)nGenerate) - T0*CLHEP::s;

  // Offset with different w.r.t. central value
  for(G4int iParticle = 0; iParticle < nGenerate; ++iParticle)
    {
      x0_v[iParticle] -= x0_a;
      xp_v[iParticle] -= xp_a;
      y0_v[iParticle] -= y0_a;
      yp_v[iParticle] -= yp_a;
      z0_v[iParticle] -= z0_a;
      zp_v[iParticle] -= zp_a;
      E_v[iParticle]  -= E_a;
      t_v[iParticle]  -= t_a;
    }
}
