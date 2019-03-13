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
#include "BDSDebug.hh"
#include "BDSBeamPipeFactoryOctagonal.hh"
#include "BDSBeamPipeFactoryPoints.hh"

#include "globals.hh"                 // geant4 globals / types
#include "G4TwoVector.hh"

#include <cmath>

BDSBeamPipeFactoryOctagonal::BDSBeamPipeFactoryOctagonal()
{;}

void BDSBeamPipeFactoryOctagonal::GenerateOctagonal(std::vector<G4TwoVector>& vec,
						    G4double x1,
						    G4double y1,
						    G4double x2,
						    G4double y2)
{
  AppendPoint(vec, x1,  y2 );
  AppendPoint(vec, x1,  -y2);
  AppendPoint(vec, x2,  -y1);
  AppendPoint(vec, -x2, -y1);
  AppendPoint(vec, -x1, -y2);
  AppendPoint(vec, -x1, y2 );
  AppendPoint(vec, -x2, y1 );
  AppendPoint(vec, x2,  y1 );
}						    

void BDSBeamPipeFactoryOctagonal::GeneratePoints(G4double aper1,
						 G4double aper2,
						 G4double aper3,
						 G4double aper4,
						 G4double beamPipeThickness,
						 G4int    /*pointsPerTwoPi*/)
{
  GenerateOctagonal(vacuumEdge, aper1, aper2, aper3, aper4);
  G4double bpInner1 = aper1 + lengthSafetyLarge;
  G4double bpInner2 = aper2 + lengthSafetyLarge;
  G4double bpInner3 = aper3 + lengthSafetyLarge;
  G4double bpInner4 = aper4 + lengthSafetyLarge;
  GenerateOctagonal(beamPipeInnerEdge, bpInner1, bpInner2, bpInner3, bpInner4);
  G4double bpOuter1 = bpInner1 + beamPipeThickness;
  G4double bpOuter2 = bpInner2 + beamPipeThickness;
  G4double bpOuter3 = bpInner3 + beamPipeThickness;
  G4double bpOuter4 = bpInner4 + beamPipeThickness;
  GenerateOctagonal(beamPipeOuterEdge, bpOuter1, bpOuter2, bpOuter3, bpOuter4);
  G4double cont1 = bpOuter1 + lengthSafetyLarge;
  G4double cont2 = bpOuter2 + lengthSafetyLarge;
  G4double cont3 = bpOuter3 + lengthSafetyLarge;
  G4double cont4 = bpOuter4 + lengthSafetyLarge;
  GenerateOctagonal(containerEdge, cont1, cont2, cont3, cont4);
  G4double contSub1 = cont1 + lengthSafetyLarge;
  G4double contSub2 = cont2 + lengthSafetyLarge;
  G4double contSub3 = cont3 + lengthSafetyLarge;
  G4double contSub4 = cont4 + lengthSafetyLarge;
  GenerateOctagonal(containerSubtractionEdge, contSub1, contSub2, contSub3, contSub4);

  extentX = contSub1;
  extentY = contSub2;
}

G4double BDSBeamPipeFactoryOctagonal::CalculateIntersectionRadius(G4double aper1,
								  G4double aper2,
								  G4double aper3,
								  G4double aper4,
								  G4double beamPipeThickness)
{
  // depending on the user supplied parameters, choose the largest radius from the origin,
  // and add the beam pipe thickness plus some margin
  G4double r1 = std::hypot(aper1,aper2);
  G4double r2 = std::hypot(aper3,aper4);
  G4double result = std::max(r1,r2) + beamPipeThickness;
  result *= 1.4;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "intersection radius: " << result << G4endl;
#endif
  return result;
}
