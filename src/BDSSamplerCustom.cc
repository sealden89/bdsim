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
#include "BDSApertureInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSamplerCustom.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDManager.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"


BDSSamplerCustom::BDSSamplerCustom(G4String               nameIn,
				   const BDSApertureInfo& shape):
  BDSSampler(nameIn)
{
  /*
  BDSApertureFactory fac;
  containerSolid = fac.CreateAperture(name + "_aperture",
				      BDSSamplerPlane::chordLength,
				      shape);
  */
  switch (shape.apertureType.underlying())
    {
    case BDSApertureType::circular:
    case BDSApertureType::circularvacuum:
      {
	containerSolid = new G4Tubs(name + "_solid",
				    0,
				    shape.aper1,
				    0.5*BDSSamplerPlane::chordLength,
				    0,
				    CLHEP::twopi);
	break;
      }
    case BDSApertureType::rectangular:
      {
	containerSolid = new G4Box(name + "_solid",
				   shape.aper1,
				   shape.aper2,
				   0.5*BDSSamplerPlane::chordLength);
	break;
      }
    default:
      {
	G4cerr << "Shape \"" << shape.apertureType.ToString() << "\" is not currently supported." << G4endl;
	G4cerr << "Please use circular or rectangular." << G4endl;
	exit(1);
	break;
      }
    }
  

  BDSExtent ae = shape.Extent();
  G4double  dz = BDSSamplerPlane::chordLength * 0.5;
  SetExtent(BDSExtent(ae.XNeg(), ae.XPos(), ae.YNeg(), ae.YPos(), -dz, dz));

  CommonConstruction();

  containerLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->GetSamplerPlaneSD());
}
