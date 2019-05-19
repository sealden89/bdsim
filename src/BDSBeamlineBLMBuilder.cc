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
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamlineBLMBuilder.hh"
#include "BDSBLM.hh"
#include "BDSBLMFactory.hh"
#include "BDSBLMRegistry.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSExtent.hh"
#include "BDSSimpleComponent.hh"

#include "parser/blmplacement.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include <vector>


BDSBeamline* BDS::BuildBLMs(const std::vector<GMAD::BLMPlacement>& blmPlacements,
			    const BDSBeamline* parentBeamLine)
{
  if (blmPlacements.empty())
    {return nullptr;} // don't do anything - no placements
  
  BDSBeamline* blms = new BDSBeamline();

  for (const auto& bp : blmPlacements)
    {
      BDSBLMFactory factory;
      BDSBLM* blm = factory.BuildBLM(bp.name,
				     bp.geometryFile,
				     bp.geometryType,
				     bp.blmMaterial,
				     bp.blm1 * CLHEP::m,
				     bp.blm2 * CLHEP::m,
				     bp.blm3 * CLHEP::m,
				     bp.blm4 * CLHEP::m);
      
      G4double length = blm->GetExtent().DZ();
      BDSSimpleComponent* comp = new BDSSimpleComponent(blm->GetName(),
							blm,
							length);

      G4double S = -1000;
      G4Transform3D transform = BDSDetectorConstruction::CreatePlacementTransform(bp, parentBeamLine, &S);
      BDSBLMRegistry::Instance()->RegisterBLM(bp.name, blm, S);
      
      /// Here we're assuming the length is along z which may not be true, but
      /// close enough for this purpose as we rely only on the centre position.
      G4ThreeVector halfLengthBeg = G4ThreeVector(0,0,-length*0.5);
      G4ThreeVector halfLengthEnd = G4ThreeVector(0,0, length*0.5);
      G4ThreeVector midPos        = transform.getTranslation();
      G4ThreeVector startPos = midPos + transform * (HepGeom::Point3D<G4double>)halfLengthBeg;
      G4ThreeVector endPos   = midPos + transform * (HepGeom::Point3D<G4double>)halfLengthEnd;
      G4RotationMatrix* rm   = new G4RotationMatrix(transform.getRotation());
      
      BDSBeamlineElement* el = new BDSBeamlineElement(comp,
						      startPos,
						      midPos,
						      endPos,
						      rm,
						      new G4RotationMatrix(*rm),
						      new G4RotationMatrix(*rm),
						      startPos,
						      midPos,
						      endPos,
						      new G4RotationMatrix(*rm),
						      new G4RotationMatrix(*rm),
						      new G4RotationMatrix(*rm),
						      -1,-1,-1);

      blms->AddBeamlineElement(el);
    }

  return blms;
}
