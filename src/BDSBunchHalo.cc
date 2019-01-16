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
#include "BDSBunchHalo.hh"
#include "BDSDebug.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSUtilities.hh"

#include "parser/beam.h"

#include "globals.hh" // geant4 types / globals

#include "Randomize.hh"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSBunchHalo::BDSBunchHalo():
  alphaX(0.0), alphaY(0.0),
  betaX(0.0), betaY(0.0),
  emitX(0.0), emitY(0.0),
  gammaX(0.0), gammaY(0.0),
  sigmaX(0.0), sigmaY(0.0),
  haloNSigmaXInner(0.0), haloNSigmaXOuter(0.0),
  haloNSigmaYInner(0.0), haloNSigmaYOuter(0.0),
  haloXCutInner(0.0), 
  haloYCutInner(0.0),
  haloPSWeightParameter(0.0),
  weightFunction(""),  
  haloNSigmaXpOuter(0.0),
  haloNSigmaYpOuter(0.0),
  emitInnerX(0.0), emitInnerY(0.0),
  emitOuterX(0.0), emitOuterY(0.0),
  xMax(0.0), yMax(0.0),
  xpMax(0.0), ypMax(0.0)
{
  flatGen = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
}

BDSBunchHalo::~BDSBunchHalo() 
{
  delete flatGen; 
}

void  BDSBunchHalo::SetOptions(const BDSParticleDefinition* beamParticle,
			       const GMAD::Beam& beam,
			       const BDSBunchType& distrType,
			       G4Transform3D beamlineTransformIn,
			       const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  alphaX                = G4double(beam.alfx);
  alphaY                = G4double(beam.alfy);
  betaX                 = G4double(beam.betx);
  betaY                 = G4double(beam.bety);
  emitX                 = G4double(beam.emitx);
  emitY                 = G4double(beam.emity);
  gammaX                = (1.0+alphaX*alphaX)/betaX;
  gammaY                = (1.0+alphaY*alphaY)/betaY;
  sigmaX                = std::sqrt(emitX * betaX);
  sigmaY                = std::sqrt(emitY * betaY);
  haloNSigmaXInner      = G4double(beam.haloNSigmaXInner);
  haloNSigmaXOuter      = G4double(beam.haloNSigmaXOuter);
  haloNSigmaYInner      = G4double(beam.haloNSigmaYInner);
  haloNSigmaYOuter      = G4double(beam.haloNSigmaYOuter);
  haloXCutInner         = G4double(beam.haloXCutInner);
  haloYCutInner         = G4double(beam.haloYCutInner);  
  haloPSWeightParameter = G4double(beam.haloPSWeightParameter);
  weightFunction = G4String(beam.haloPSWeightFunction);  

  haloNSigmaXpOuter     = std::sqrt(gammaX * emitX);
  haloNSigmaYpOuter     = std::sqrt(gammaY * emitY);   

  emitInnerX = std::pow(haloNSigmaXInner, 2) * emitX;
  emitInnerY = std::pow(haloNSigmaYInner, 2) * emitY;
  emitOuterX = std::pow(haloNSigmaXOuter, 2) * emitX;
  emitOuterY = std::pow(haloNSigmaYOuter, 2) * emitY;  

  xMax  = haloNSigmaXOuter * sigmaX;
  yMax  = haloNSigmaYOuter * sigmaY;
  xpMax = std::sqrt(std::pow(haloNSigmaXOuter, 2) * emitX * gammaX);
  ypMax = std::sqrt(std::pow(haloNSigmaYOuter, 2) * emitY * gammaY);
  
  CheckParameters(); 
}

BDSParticleCoordsFull BDSBunchHalo::GetNextParticleLocal()  
{
  // Central orbit 
  G4double x = X0;
  G4double y = Y0;
  G4double z = Z0;
  G4double xp = Xp0;
  G4double yp = Yp0;

  //  z += (T0 - envelopeT * (1.-2.*flatGen->shoot())) * CLHEP::c_light * CLHEP::s;
  z = 0;

  while(true)
  {
    G4double dx  = xMax  * (1 - 2 * flatGen->shoot());
    G4double dy  = yMax  * (1 - 2 * flatGen->shoot());
    G4double dxp = xpMax * (1 - 2 * flatGen->shoot());
    G4double dyp = ypMax * (1 - 2 * flatGen->shoot());
    
    // compute single particle emittance 
    double emitXSp = gammaX * std::pow(std::abs(dx), 2) + (2. * alphaX * dx * dxp) + betaX * std::pow(std::abs(dxp), 2);
    double emitYSp = gammaY * std::pow(std::abs(dy), 2) + (2. * alphaY * dy * dyp) + betaY * std::pow(std::abs(dyp), 2);

#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "phase space> " << dx << " " << dy << " " << dxp << " " << dyp << G4endl;
    G4cout << __METHOD_NAME__ << "Xcollimators> "  << haloXCutInner << G4endl;
    G4cout << __METHOD_NAME__ << "Ycollimators> "  << haloYCutInner << G4endl;
    G4cout << __METHOD_NAME__ << "emittance> " << emitX << " " << emitXSp << " " << emitInnerX << " " << emitOuterX << " "
	   << emitY << " " << emitYSp << " " << emitInnerY << " " << emitOuterY << G4endl;
#endif

    // check if particle is within normal beam core, if so continue generation
    // also check if particle is within the desired cut.
    if ((std::abs(emitXSp) < emitInnerX || std::abs(emitYSp) < emitInnerY) ||
	(std::abs(emitXSp) > emitOuterX || std::abs(emitYSp) > emitOuterY)  ||
        (std::abs(dx)  < (haloXCutInner * sigmaX)) ||
	(std::abs(dy)  < (haloYCutInner * sigmaY)) )
      {
	continue;
      }
    else
      {
	// determine weight, initialise 1 so always passes
	double wx = 1.0;
	double wy = 1.0;
	if (weightFunction == "flat" || weightFunction == "" || weightFunction == "one")
	  {
	    wx = 1.0;
	    wy = 1.0;
	  }
	else if (weightFunction == "oneoverr")
	  {
	    //abs because power of double - must be positive
	    wx = std::pow(std::abs(emitInnerX / emitXSp), haloPSWeightParameter);
	    wy = std::pow(std::abs(emitInnerY / emitYSp), haloPSWeightParameter);
	  }
	else if (weightFunction == "oneoverrsqrd")
	  {
	    //abs because power of double - must be positive
	    double eXsqrd = std::pow(std::abs(emitXSp), 2);
	    double eYsqrd = std::pow(std::abs(emitYSp), 2);
	    double eXInsq = std::pow(std::abs(emitInnerX), 2);
	    double eYInsq = std::pow(std::abs(emitInnerY), 2);
	    wx = std::pow(std::abs(eXInsq / eXsqrd), haloPSWeightParameter);
	    wy = std::pow(std::abs(eYInsq / eYsqrd), haloPSWeightParameter);
	  }
	else if (weightFunction == "exp")
	  {
	    wx = std::exp(-(emitXSp * haloPSWeightParameter) / (emitInnerX));
	    wy = std::exp(-(emitYSp * haloPSWeightParameter) / (emitInnerY));
	  }
	
#ifdef BDSDEBUG
	G4cout << __METHOD_NAME__ << emitXSp/emitX << " " << emitYSp/emitY << " " << wx << " " << wy << G4endl;
#endif
	// reject
	if(flatGen->shoot() > wx && flatGen->shoot() > wy)
	  {continue;}
	
	// add to reference orbit 
	x += dx * CLHEP::m;
	y += dy * CLHEP::m;
	xp += dxp * CLHEP::rad;
	yp += dyp * CLHEP::rad;
	
	G4double zp = CalculateZp(xp, yp, Zp0);	
	G4double t = T0 * CLHEP::s;
	G4double E = E0 * CLHEP::GeV;
	
#ifdef BDSDEBUG
	G4cout << __METHOD_NAME__ << "selected> " << dx << " " << dy << " " << dxp << " " << dyp << G4endl;
#endif
	BDSParticleCoordsFull result(x,y,z,xp,yp,zp,t,S0+z,E,/*weight=*/1.0);
	return result;
      }
  }
}

void BDSBunchHalo::CheckParameters()
{
  BDSBunch::CheckParameters();
  
  if (emitX <= 0)
    {G4cerr << __METHOD_NAME__ << "emitx must be finite!" << G4endl; exit(1);}
  if (emitY <= 0)
    {G4cerr << __METHOD_NAME__ << "emity must be finite!" << G4endl; exit(1);}
  
  std::vector<G4String> weightFunctions = {"", "one", "flat","oneoverr", "oneoverrsqrd", "exp"};
  auto search = std::find(weightFunctions.begin(), weightFunctions.end(), weightFunction);
  if (search == weightFunctions.end())
    {
      G4cerr << __METHOD_END__ << "invalid haloPSWeightFunction \"" << weightFunction << "\"" << G4endl;
      G4cout << "Available weight functions are:" << G4endl;
      for (const auto& w : weightFunctions)
        {G4cout << w << G4endl;}
      exit(1);
    }
  
  if (haloNSigmaXInner <= 0)
    {G4cerr << __METHOD_NAME__ << "haloNSigmaXInner <= 0" << G4endl; exit(1);}
  
  if (haloNSigmaYInner <= 0)
    {G4cerr << __METHOD_NAME__ << "haloYSigmaXInner <= 0" << G4endl; exit(1);}
  
  if (haloNSigmaXInner > haloNSigmaXOuter)
    {G4cerr << __METHOD_NAME__ << "haloNSigmaXInner cannot be less than haloNSigmaXOuter" << G4endl; exit(1);}
  
  if (haloNSigmaYInner > haloNSigmaYOuter)
    {G4cerr << __METHOD_NAME__ << "haloNSigmaYInner cannot be less than haloNSigmaYOuter" << G4endl; exit(1);} 
}
