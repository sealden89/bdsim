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
#include "BDSBeamPipeFactoryPoints.hh"
#include "BDSBeamPipe.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSUtilities.hh"

#include "globals.hh"                 // geant4 globals / types
#include "G4CutTubs.hh"
#include "G4ExtrudedSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"
#include "G4VSolid.hh"

#include <cmath>                           // sin, cos, fabs
#include <set>
#include <utility>                         // for std::pair
#include <vector>

BDSBeamPipeFactoryPoints::BDSBeamPipeFactoryPoints()
{
  CleanUpPoints();
}

BDSBeamPipeFactoryPoints::~BDSBeamPipeFactoryPoints()
{;}

void BDSBeamPipeFactoryPoints::CleanUp()
{
  CleanUpPoints();
  BDSBeamPipeFactoryBase::CleanUp();
}

void BDSBeamPipeFactoryPoints::CleanUpPoints()
{
  intersectionRadius = 0;
  extentX            = 0;
  extentY            = 0;
  
  vacuumEdge.clear();
  beamPipeInnerEdge.clear();
  beamPipeOuterEdge.clear();
  containerEdge.clear();
  containerSubtractionEdge.clear();

  beamPipeInnerSolid = nullptr;
  beamPipeOuterSolid = nullptr;
}

void BDSBeamPipeFactoryPoints::AppendPoint(std::vector<G4TwoVector>& vec,
					   G4double x,
					   G4double y)
{
  vec.push_back(G4TwoVector(x,y));
}

void BDSBeamPipeFactoryPoints::AppendAngle(std::vector<G4TwoVector>& vec,
					   G4double startAngle,
					   G4double finishAngle,
					   G4double radius,
					   G4int    nPoints,
					   G4double xOffset,
					   G4double yOffset)
{
  AppendAngleEllipse(vec, startAngle, finishAngle, radius, radius,nPoints, xOffset, yOffset);
}

void BDSBeamPipeFactoryPoints::AppendAngleEllipse(std::vector<G4TwoVector>& vec,
						  G4double startAngle,
						  G4double finishAngle,
						  G4double radiusA,
						  G4double radiusB,
						  G4int    nPoints,
						  G4double xOffset,
						  G4double yOffset)
  {
  G4double diff = finishAngle - startAngle;
  G4double delta = diff / (G4double)nPoints;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "start angle:  " << startAngle  << G4endl;
  G4cout << __METHOD_NAME__ << "finish angle: " << finishAngle << G4endl;
  G4cout << __METHOD_NAME__ << "# of points:  " << nPoints     << G4endl;
  G4cout << __METHOD_NAME__ << "diff angle:   " << diff        << G4endl;
  G4cout << __METHOD_NAME__ << "delta angle:  " << delta       << G4endl;
#endif
  for (G4double ang = startAngle; ang < finishAngle; ang += delta)
    { // l for local
      G4double xl = xOffset + radiusA*std::sin(ang);
      G4double yl = yOffset + radiusB*std::cos(ang);
      AppendPoint(vec, xl, yl);
    }
}

void BDSBeamPipeFactoryPoints::CreateSolids(G4String name,
					    G4double length,
					    G4bool   buildLongForIntersection)
{
  /// Make the solids all longer for intersection
  G4double zHalfLength = 0.5*length;
  if (buildLongForIntersection)
    {zHalfLength *= 1.5;}
  
  G4TwoVector zOffsets(0,0); // the transverse offset of each plane from 0,0
  G4double zScale = 1; // the scale at each end of the points = 1
  vacuumSolid = new G4ExtrudedSolid(name + "_vacuum_solid", // name
				    vacuumEdge,             // vector of TwoVector points
				    zHalfLength - lengthSafety, // half length for +- planes
				    zOffsets, zScale,       // dx,dy offset for each face, scaling
				    zOffsets, zScale);      // dx,dy offset for each face, scaling

  beamPipeInnerSolid = new G4ExtrudedSolid(name + "_bp_inner_solid",
					   beamPipeInnerEdge,
					   zHalfLength * 4,
					   zOffsets, zScale,
					   zOffsets, zScale);
  
  beamPipeOuterSolid = new G4ExtrudedSolid(name + "_bp_outer_solid",
					   beamPipeOuterEdge,
					   zHalfLength - 4*lengthSafety,
					   zOffsets, zScale,
					   zOffsets, zScale);

  allSolids.insert(beamPipeInnerSolid);
  allSolids.insert(beamPipeOuterSolid);
  
  beamPipeSolid = new G4SubtractionSolid(name + "_pipe_solid", // name
					 beamPipeOuterSolid,   // this
					 beamPipeInnerSolid);  // minus this

  containerSolid = new G4ExtrudedSolid(name + "_container_solid",
				       containerEdge,
				       zHalfLength,
				       zOffsets, zScale,
				       zOffsets, zScale);

  containerSubtractionSolid = new G4ExtrudedSolid(name + "_container_subtraction_solid",
						  containerSubtractionEdge,
						  zHalfLength*4,
						  zOffsets, zScale,
						  zOffsets, zScale);
}

void BDSBeamPipeFactoryPoints::CreateSolidsAngled(G4String      name,
						  G4double      length,
						  G4ThreeVector inputFace,
						  G4ThreeVector outputFace)
{
  // create straight solids that are a bit long
  CreateSolids(name + "_straight", length, true);

  // now intersect them with one G4CutTubs to get the angled faces
  G4double zHalfLength          = length*0.5 - lengthSafety;
  G4double zHalfLengthContainer = length*0.5;
  
  G4VSolid* faceSolid = new G4CutTubs(name + "_face_solid", // name
				      0,                    // inner radius
				      intersectionRadius,   // outer radius
				      zHalfLength,          // z half length
				      0,                    // start angle
				      CLHEP::twopi,         // sweep angle
				      inputFace,            // input face normal
				      outputFace);          // output face normal

  // different only by container length > length of inner section
  G4VSolid* faceSolidContainer = new G4CutTubs(name + "_cont_face_solid", // name
					       0,                         // inner radius
					       intersectionRadius,        // outer radius
					       zHalfLengthContainer,      // z half length
					       0,                         // start angle
					       CLHEP::twopi,              // sweep angle
					       inputFace,                 // input face normal
					       outputFace);               // output face normal

  allSolids.insert(faceSolid);
  allSolids.insert(faceSolidContainer);

  // copy pointers to do intersection with then reassign member pointer
  // to point to new solid
  G4VSolid* vacuumTemp = vacuumSolid;
  vacuumSolid = new G4IntersectionSolid(name + "_vacuum_solid",
					vacuumTemp,
					faceSolid);

  G4VSolid* beamPipeTemp = beamPipeSolid;
  beamPipeSolid = new G4IntersectionSolid(name + "_pipe_solid",
					  beamPipeTemp,
					  faceSolid);

  G4VSolid* containerTemp = containerSolid;
  containerSolid = new G4IntersectionSolid(name + "_container_solid",
					   containerTemp,
					   faceSolidContainer);

  // container subtraction solid can just be long with flat edges as
  // only used transversely
}

BDSBeamPipe* BDSBeamPipeFactoryPoints::CreateBeamPipe(G4String    nameIn,
						      G4double    lengthIn,
						      G4double    aper1In,
						      G4double    aper2In,
						      G4double    aper3In,
						      G4double    aper4In,
						      G4Material* vacuumMaterialIn,
						      G4double    beamPipeThicknessIn,
						      G4Material* beamPipeMaterialIn)
{
  // clean up after last usage
  CleanUp();
  
  // generate extruded solid edges - provided by derived class
  GeneratePoints(aper1In, aper2In, aper3In, aper4In, beamPipeThicknessIn);

  // calculate and set the intersection solid radius
  intersectionRadius = CalculateIntersectionRadius(aper1In, aper2In, aper3In, aper4In, beamPipeThicknessIn);

  // create solids based on the member vectors of points
  CreateSolids(nameIn, lengthIn);
					
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn);
}

BDSBeamPipe* BDSBeamPipeFactoryPoints::CreateBeamPipe(G4String      nameIn,
						      G4double      lengthIn,
						      G4ThreeVector inputFaceNormalIn,
						      G4ThreeVector outputFaceNormalIn,
						      G4double      aper1In,
						      G4double      aper2In,
						      G4double      aper3In,
						      G4double      aper4In,
						      G4Material*   vacuumMaterialIn,
						      G4double      beamPipeThicknessIn,
						      G4Material*   beamPipeMaterialIn)
{
  // clean up after last usage
  CleanUp();
  
  // generate extruded solid edges - provided by derived class
  GeneratePoints(aper1In, aper2In, aper3In, aper4In, beamPipeThicknessIn);
  
  inputFaceNormal  = inputFaceNormalIn;
  outputFaceNormal = outputFaceNormalIn;

  // calculate and set the intersection solid radius
  intersectionRadius = CalculateIntersectionRadius(aper1In, aper2In, aper3In, aper4In,
						   beamPipeThicknessIn);

  // create solids based on the member vectors of points
  CreateSolidsAngled(nameIn, lengthIn, inputFaceNormal, outputFaceNormal);
  
  return CommonFinalConstruction(nameIn, vacuumMaterialIn, beamPipeMaterialIn, lengthIn);
}

BDSBeamPipe* BDSBeamPipeFactoryPoints::CommonFinalConstruction(G4String    nameIn,
							       G4Material* vacuumMaterialIn,
							       G4Material* beamPipeMaterialIn,
							       G4double    lengthIn)
{
  BDSBeamPipeFactoryBase::CommonConstruction(nameIn, vacuumMaterialIn,
					     beamPipeMaterialIn, lengthIn);

  // record extents
  BDSExtent ext = BDSExtent(extentX, extentY, lengthIn*0.5);
  
  // calculate radius if a tube were to be place around it
  G4double containerRadius = intersectionRadius;
  
  BDSBeamPipe* aPipe = BuildBeamPipeAndRegisterVolumes(ext,containerRadius);
  
  return aPipe;
}
