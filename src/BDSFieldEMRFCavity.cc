/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#include "BDSCavityInfo.hh"
#include "BDSFieldEMRFCavity.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include "TMath.h"

#include <cmath>
#include <utility>

const G4double BDSFieldEMRFCavity::j0FirstZero = 2.404825557695772768622;

const G4double BDSFieldEMRFCavity::Z0 = CLHEP::mu0 * CLHEP::c_light;

BDSFieldEMRFCavity::BDSFieldEMRFCavity(BDSMagnetStrength const* strength,
				       G4double                 brho):
  BDSFieldEMRFCavity((*strength)["efield"],
		     (*strength)["frequency"],
		     (*strength)["phase"],
		     (*strength)["equatorradius"])
{
  eFieldMax *= BDS::Sign(brho);
}

BDSFieldEMRFCavity::BDSFieldEMRFCavity(G4double eFieldAmplitude,
				       G4double frequencyIn,
				       G4double phaseOffset,
				       G4double cavityRadiusIn):
  eFieldMax(eFieldAmplitude),
  frequency(frequencyIn),
  phase(phaseOffset),
  cavityRadius(cavityRadiusIn),
  normalisedCavityRadius(j0FirstZero/cavityRadius),
  angularFrequency(CLHEP::twopi * frequencyIn)
{;}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMRFCavity::GetField(const G4ThreeVector& position,
                                                                     const G4double       t) const
{
  // Converting from Local Cartesian to Local Cylindrical
  G4double phi = std::atan2(position.y(),position.x());
  G4double r   = std::hypot(position.x(),position.y());

  G4double rNormalised = normalisedCavityRadius * r;

  // In case a point outside the cavity is queried, ensure the bessel will return 0
  if (rNormalised > j0FirstZero)
    {rNormalised = j0FirstZero - 1e-6;}

  // Source for calculating the TM010 mode: Gerigk, Frank.
  // "Cavity types." arXiv preprint arXiv:1111.4897 (2011).

  G4double J0r = TMath::BesselJ0(rNormalised);
  G4double J1r = TMath::BesselJ1(rNormalised);

  // Calculating free-space impedance and scale factor for Bphi:
  G4double hMax = -eFieldMax/Z0;
  G4double Bmax = hMax * CLHEP::mu0;

  // Calculating field components.  Frequency in rad/s or /s?
  G4double Ez   = eFieldMax * J0r * std::cos(angularFrequency*t + phase);
  G4double Bphi = Bmax * J1r * std::sin(angularFrequency*t + phase);

  // Converting Bphi into cartesian coordinates:
  G4double Bx = Bphi*std::sin(phi);
  G4double By = Bphi*std::cos(phi);

  // Local B and E fields:
  G4ThreeVector LocalB = G4ThreeVector(Bx, By, 0);
  G4ThreeVector LocalE = G4ThreeVector(0,  0,  Ez);
  
  auto result = std::make_pair(LocalB, LocalE);
  return result;
}
