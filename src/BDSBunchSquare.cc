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
#include "BDSBunchSquare.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSParticleCoordsFull.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Units/PhysicalConstants.h"

BDSBunchSquare::BDSBunchSquare():
  envelopeX(0.0),
  envelopeY(0.0),
  envelopeXp(0.0),
  envelopeYp(0.0),
  envelopeT(0.0),
  envelopeE(0.0)
{
  flatGen = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
}

BDSBunchSquare::~BDSBunchSquare()
{
  delete flatGen;
}

void BDSBunchSquare::SetOptions(const BDSParticleDefinition* beamParticle,
				const GMAD::Beam& beam,
				const BDSBunchType& distrType,
				G4Transform3D beamlineTransformIn,
				const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  envelopeX  = beam.envelopeX  * CLHEP::m;
  envelopeY  = beam.envelopeY  * CLHEP::m;
  envelopeXp = beam.envelopeXp * CLHEP::rad;
  envelopeYp = beam.envelopeYp * CLHEP::rad;
  envelopeT  = beam.envelopeT  * CLHEP::s;
  envelopeE  = beam.envelopeE  * CLHEP::GeV;
}

void BDSBunchSquare::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (envelopeX <  0)
    {throw BDSException(__METHOD_NAME__, "envelopeX < 0");}
  if (envelopeXp < 0)
    {throw BDSException(__METHOD_NAME__, "envelopeXp < 0");}
  if (envelopeY <  0)
    {throw BDSException(__METHOD_NAME__, "envelopeY < 0");}
  if (envelopeYp < 0)
    {throw BDSException(__METHOD_NAME__, "envelopeYp < 0");}
  if (envelopeT < 0)
    {throw BDSException(__METHOD_NAME__, "envelopeT < 0");}
  if (envelopeE < 0)
    {throw BDSException(__METHOD_NAME__, "envelopeE < 0");}
}

BDSParticleCoordsFull BDSBunchSquare::GetNextParticleLocal()
{  
  G4double x  = X0  + envelopeX  * (1-2*flatGen->shoot());
  G4double y  = Y0  + envelopeY  * (1-2*flatGen->shoot());
  G4double xp = Xp0 + envelopeXp * (1-2*flatGen->shoot());
  G4double yp = Yp0 + envelopeYp * (1-2*flatGen->shoot());
  G4double zp = CalculateZp(xp,yp,Zp0);
  G4double dt = envelopeT * (1.-2.*flatGen->shoot());
  G4double t  = T0 + dt;
  G4double dz = dt * CLHEP::c_light;
  G4double z  = Z0 + dz;
  G4double E  = E0 + envelopeE * (1 - 2*flatGen->shoot());
  
  return BDSParticleCoordsFull(x,y,z,xp,yp,zp,t,S0+dz,E,/*weight=*/1.0);
}
