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
#include "BDSBunchCircle.hh"
#include "BDSDebug.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSBunchCircle::BDSBunchCircle():
  BDSBunch(),
  envelopeR(0.0),
  envelopeRp(0.0),
  envelopeT(0.0),
  envelopeE(0.0)
{
  flatGen = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
}

BDSBunchCircle::~BDSBunchCircle() 
{
  delete flatGen;
}

void BDSBunchCircle::SetOptions(const BDSParticleDefinition* beamParticle,
				const GMAD::Beam& beam,
				const BDSBunchType& distrType,
				G4Transform3D beamlineTransformIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn);
  envelopeR  = beam.envelopeR  * CLHEP::m; 
  envelopeRp = beam.envelopeRp * CLHEP::rad;
  envelopeT  = beam.envelopeT  * CLHEP::s;
  envelopeE  = beam.envelopeE; 
}

void BDSBunchCircle::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (envelopeR <= 0)
    {G4cerr << __METHOD_NAME__ << "envelopeR <=0 "  << G4endl; exit(1);}
  if (envelopeRp <= 0)
    {G4cerr << __METHOD_NAME__ << "envelopeRp <=0 " << G4endl; exit(1);}
  if (envelopeT < 0)
    {G4cerr << __METHOD_NAME__ << "envelopeT < 0 "  << G4endl; exit(1);}
  if (envelopeE < 0)
    {G4cerr << __METHOD_NAME__ << "envelopeE < 0 "  << G4endl; exit(1);}
}

BDSParticleCoordsFull BDSBunchCircle::GetNextParticleLocal()
{
  G4double t     = T0 - envelopeT * (1.-2.*flatGen->shoot());
  G4double z0    = Z0 + t * CLHEP::c_light;
  G4double phiR  = flatGen->shoot() * CLHEP::twopi;
  G4double phiRp = flatGen->shoot() * CLHEP::twopi;
  G4double r     = flatGen->shoot() * envelopeR;
  G4double rp    = flatGen->shoot() * envelopeRp; 

  G4double x0 = X0  + std::cos(phiR)  * r;
  G4double y0 = Y0  + std::sin(phiR)  * r;
  G4double xp = Xp0 + std::cos(phiRp) * rp;
  G4double yp = Yp0 + std::sin(phiRp) * rp; 
  G4double zp = CalculateZp(xp,yp,Zp0);
  G4double E  = E0 * (1 + envelopeE * (1-2*flatGen->shoot()));

  return BDSParticleCoordsFull(x0,y0,z0,xp,yp,zp,t,S0,E,/*weight=*/1.0);
}
