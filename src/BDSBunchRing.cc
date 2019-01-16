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
#include "BDSBunchRing.hh"
#include "BDSDebug.hh"
#include "BDSParticleCoordsFull.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSBunchRing::BDSBunchRing(): 
  rMin(0),
  rMax(0),
  rDif(0)
{
  flatGen = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());  
}

BDSBunchRing::~BDSBunchRing()
{
  delete flatGen;
}

void BDSBunchRing::SetOptions(const BDSParticleDefinition* beamParticle,
			      const GMAD::Beam& beam,
			      const BDSBunchType& distrType,
			      G4Transform3D beamlineTransformIn,
			      const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  rMin = beam.Rmin * CLHEP::m;
  rMax = beam.Rmax * CLHEP::m;
  rDif = rMax - rMin;
}

void BDSBunchRing::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (rMin < 0)
    {G4cerr << __METHOD_NAME__ << "rMin: " << rMin << " < 0" << G4endl; exit(1);}
  if (rMax < 0)
    {G4cerr << __METHOD_NAME__ << "rMax: " << rMin << " < 0" << G4endl; exit(1);}
  if (rMax <= rMin)
    {G4cerr << __METHOD_NAME__ << "rMax: " << rMax << " < rMin: " << rMin << G4endl; exit(1);}
}

BDSParticleCoordsFull BDSBunchRing::GetNextParticleLocal()
{
  G4double r   = rMin + rDif * flatGen->shoot();
  G4double phi = 2 * CLHEP::pi * flatGen->shoot();
  G4double x   = X0 + r * std::sin(phi);
  G4double y   = Y0 + r * std::cos(phi);
  
  return BDSParticleCoordsFull(x,y,Z0,Xp0,Yp0,Zp0,T0,S0,E0,/*weight=*/1.0);
}

