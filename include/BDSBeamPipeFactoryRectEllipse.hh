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
#ifndef BDSBEAMPIPEFACTORYRECTELLIPSE_H
#define BDSBEAMPIPEFACTORYRECTELLIPSE_H

#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipe.hh"

/**
 * @brief Factory for rectellipse aperture model beampipes.
 * 
 * This is the overlap of (boolean AND) a rectangle and an ellipse
 * transversely.  This factory is similar to the simple LHC factory
 * and uses the intersection of an elliptical tube and a box. This
 * method produces a solid shape, so to achieve a pipe, a smaller
 * duplicate intersection solid is made for subtraction from the outer
 * one. To achieve the angled faces, G4CutTubs is used and the rectangular
 * box is made longer so the first intersection gives the angled face
 * from the easier G4CutTubs primitive.
 * 
 * This factory requires four aperture parameters. Aper1,2 are the horizontal
 * and vertical half widths of the rectangle respectively, and aper3,4 are the
 * horizontal and vertical semi-axes of the ellipse. Note, will aper1,2 should be
 * < aper1,2 otherwise an elliptical tube will be more efficient.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryRectEllipse: public BDSBeamPipeFactoryBase
{
public:
  BDSBeamPipeFactoryRectEllipse();
  virtual ~BDSBeamPipeFactoryRectEllipse(){;}

  virtual BDSBeamPipe* CreateBeamPipe(G4String    nameIn,
				      G4double    lengthIn,
				      G4double    aper1               = 0,
				      G4double    aper2               = 0,
				      G4double    aper3               = 0,
				      G4double    aper4               = 0,
				      G4Material* vacuumMaterialIn    = nullptr,
				      G4double    beamPipeThicknessIn = 0,
				      G4Material* beamPipeMaterialIn  = nullptr);
  
  virtual BDSBeamPipe* CreateBeamPipe(G4String      nameIn,
				      G4double      lengthIn,
				      G4ThreeVector inputFaceNormalIn,
				      G4ThreeVector outputFaceNormalIn,
				      G4double      aper1               = 0,
				      G4double      aper2               = 0,
				      G4double      aper3               = 0,
				      G4double      aper4               = 0,
				      G4Material*   vacuumMaterialIn    = nullptr,
				      G4double      beamPipeThicknessIn = 0,
				      G4Material*   beamPipeMaterialIn  = nullptr);

private:
  /// only the solids are unique, once we have those, the logical volumes and placement in the
  /// container are the same.  group all this functionality together
  BDSBeamPipe* CommonFinalConstruction(G4String    nameIn,
				       G4Material* vacuumMaterialIn,
				       G4Material* beamPipeMaterialIn,
				       G4double    lengthIn,
				       G4double    widthIn,
				       G4double    heightIn);
  void CreateGeneralAngledSolids(G4String      nameIn,
				 G4double      lengthIn,
				 G4double      aper1In,
				 G4double      aper2In,
				 G4double      aper3In,
				 G4double      aper4In,
				 G4double      beamPipeThicknessIn,
				 G4ThreeVector inputfaceIn,
				 G4ThreeVector outputfaceIn);

  void CreateContainerSubtractionSolid(G4String& nameIn,
				       G4double& lengthIn,
				       G4double& beamPipeThicknessIn,
				       G4double& aper1In,
				       G4double& aper2In,
				       G4double& aper3In,
				       G4double& aper4In);

};
  
#endif
