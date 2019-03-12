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
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBunch.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParticleCoords.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSParticleCoordsFullGlobal.hh"
#include "BDSParticleDefinition.hh"
#include "BDSUtilities.hh"

#include "parser/beam.h"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Geometry/Point3D.h"


BDSBunch::BDSBunch():
  X0(0.0), Y0(0.0), Z0(0.0), S0(0.0), T0(0.0), 
  Xp0(0.0), Yp0(0.0), Zp0(0.0), E0(0.0), P0(0.0),
  sigmaT(0.0), sigmaP(0.0), sigmaE(0.0),
  useCurvilinear(false),
  particleCanBeDifferent(false),
  particleDefinition(nullptr),
  finiteSigmaE(true),
  finiteSigmaT(true),
  generatePrimariesOnly(false),
  beamlineTransform(G4Transform3D()),
  mass2(0.0),
  beamline(nullptr)
{;}

G4double BDSBunch::EFromP(const G4double &pIn) const
{//E2 = p2 + m2
  return std::sqrt(std::pow(pIn,2) + mass2);
}

void BDSBunch::SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& /*distrType*/,
			  G4Transform3D beamlineTransformIn,
			  G4double beamlineSIn)
{
  particleDefinition = beamParticle;

  // back the starting point up by length safety to avoid starting on a boundary
  G4ThreeVector unitZBeamline = G4ThreeVector(0,0,-1).transform(beamlineTransformIn.getRotation());
  G4ThreeVector translation   = BDSGlobalConstants::Instance()->LengthSafety() * unitZBeamline;
  beamlineTransform = G4Transform3D(beamlineTransformIn.getRotation(), beamlineTransformIn.getTranslation()+translation);

  beamlineS          = beamlineSIn;

  X0     = beam.X0 * CLHEP::m;
  Y0     = beam.Y0 * CLHEP::m;
  Z0     = beam.Z0 * CLHEP::m;
  S0     = beam.S0 * CLHEP::m;
  T0     = beam.T0 * CLHEP::s;
  Xp0    = beam.Xp0 * CLHEP::rad;
  Yp0    = beam.Yp0 * CLHEP::rad;
  E0     = beam.E0  * CLHEP::GeV;
  sigmaE = beam.sigmaE;
  sigmaT = beam.sigmaT;

  finiteSigmaE = BDS::IsFinite(sigmaE);
  finiteSigmaT = BDS::IsFinite(sigmaT);
  
  // calculate momentum - used by some generators
  G4double mass = beamParticle->Mass();
  mass2 = std::pow(mass,2);
  if (E0 <= mass)
    {
      G4cerr << __METHOD_NAME__ << "E0 (central total energy) " << E0
	     << " GeV lower than mass of particle! " << mass << " GeV" << G4endl;
      exit(1);
    }
  P0 = std::sqrt(std::pow(E0,2) - mass2); // E^2 = p^2 + m^2
  sigmaP = (1./std::pow(beamParticle->Beta(),2)) * sigmaE; // dE/E = 1/(beta^2) dP/P
  if (finiteSigmaE)
    {G4cout << __METHOD_NAME__ << "sigmaE = " << sigmaE << " -> sigmaP = " << sigmaP << G4endl;}

  Zp0 = CalculateZp(Xp0,Yp0,beam.Zp0);

  if (S0 > beamlineS)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "using curvilinear transform" << G4endl;
#endif
      if (BDS::IsFinite(Z0))
	{
	  G4cerr << __METHOD_NAME__ << "both Z0 and S0 are defined - please define only one!" << G4endl;
	  exit(1);
	}
      useCurvilinear = true;
    } 
}

void BDSBunch::CheckParameters()
{
  if (sigmaE < 0)
    {G4cerr << __METHOD_NAME__ << "sigmaE " << sigmaE << " < 0!" << G4endl; exit(1);}
  if (sigmaT < 0)
    {G4cerr << __METHOD_NAME__ << "sigmaT " << sigmaT << " < 0!" << G4endl; exit(1);}
}

BDSParticleCoordsFullGlobal BDSBunch::GetNextParticle()
{
  BDSParticleCoordsFull local = GetNextParticleLocal();
  BDSParticleCoordsFullGlobal all = ApplyTransform(local);
  return all;
}

BDSParticleCoordsFull BDSBunch::GetNextParticleLocal()
{
  BDSParticleCoordsFull local(X0,  Y0,  Z0,
			      Xp0, Yp0, Zp0,
			      S0, T0, E0, /*weight=*/1.0);
  return local;
}

void BDSBunch::BeginOfRunAction(G4int /*numberOfEvents*/)
{;}

void BDSBunch::EndOfRunAction()
{;}

void BDSBunch::SetGeneratePrimariesOnly(G4bool generatePrimariesOnlyIn)
{generatePrimariesOnly = generatePrimariesOnlyIn;}

BDSParticleCoordsFullGlobal BDSBunch::ApplyTransform(const BDSParticleCoordsFull& localIn) const
{
  if (useCurvilinear) // i.e. S0 is finite - use beam line
    {return ApplyCurvilinearTransform(localIn);}
  else // just general beam line offset
    {return BDSParticleCoordsFullGlobal(localIn,(BDSParticleCoords)localIn.ApplyTransform(beamlineTransform));}
}

BDSParticleCoordsFullGlobal BDSBunch::ApplyCurvilinearTransform(const BDSParticleCoordsFull& localIn) const
{
  if (generatePrimariesOnly) // no beam line built so no possible transform
    {return BDSParticleCoordsFullGlobal(localIn, (BDSParticleCoords)localIn);}

  if (!beamline)
    {// initialise cache of beam line pointer
      beamline = BDSAcceleratorModel::Instance()->BeamlineMain();
      if (!beamline)
	{throw BDSException(__METHOD_NAME__, "ERROR no beamline constructed!");}
    }
  
  // 'c' for curvilinear
  G4int beamlineIndex = 0;
  G4Transform3D cTrans = beamline->GetGlobalEuclideanTransform(S0 + localIn.z,
							       localIn.x,
							       localIn.y,
							       &beamlineIndex);
  // rotate the momentum vector
  G4ThreeVector cMom = G4ThreeVector(localIn.xp, localIn.yp, localIn.zp).transform(cTrans.getRotation());
  // translation contains displacement from origin already
  G4ThreeVector cPos = cTrans.getTranslation();

  BDSParticleCoords global = BDSParticleCoords(cPos.x(), cPos.y(), cPos.z(),
					       cMom.x(), cMom.y(), cMom.z(),
					       localIn.T);

  BDSParticleCoordsFullGlobal result = BDSParticleCoordsFullGlobal(localIn, global);
  result.beamlineIndex = beamlineIndex;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << result << G4endl;
#endif
  return result;
}

G4double BDSBunch::CalculateZp(G4double xp, G4double yp, G4double Zp0In)const
{
  G4double zp;
  G4double transMom = std::pow(xp, 2) + std::pow(yp, 2);

  if (transMom > 1)
    {
      G4cout << __METHOD_NAME__ << "ERROR xp, yp too large, xp: " << xp << " yp: " << yp << G4endl;
      exit(1);
    }
  if (Zp0In < 0)
    {zp = -std::sqrt(1.0 - transMom);}
  else
    {zp = std::sqrt(1.0 - transMom);}

  return zp;
}
