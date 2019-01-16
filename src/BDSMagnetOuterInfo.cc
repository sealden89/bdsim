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
#include "BDSColours.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetOuterInfo.hh"

#include "globals.hh"             // geant4 globals / types
#include "G4Colour.hh"
#include "G4Material.hh"

BDSMagnetOuterInfo::BDSMagnetOuterInfo():
  name("not_specified"),
  geometryType(BDSMagnetGeometryType::cylindrical),
  horizontalWidth(0),
  outerMaterial(nullptr),
  innerRadius(1.0),
  vhRatio(1.0),
  angleIn(0),
  angleOut(0),
  yokeOnLeft(false),
  hStyle(false),
  buildEndPieces(true),
  coilWidthFraction(0.65),
  coilHeightFraction(0.8),
  geometryTypeAndPath("")
{
  colour = BDSColours::Instance()->GetColour("default");
}

BDSMagnetOuterInfo::BDSMagnetOuterInfo(G4String              nameIn,
				       BDSMagnetGeometryType geometryTypeIn,
				       G4double              horizontalWidthIn,
				       G4Material*           outerMaterialIn,
				       G4double              innerRadiusIn,
				       G4double              vhRatioIn,
				       G4double              angleInIn,
				       G4double              angleOutIn,
				       G4bool                yokeOnLeftIn,
				       G4bool                hStyleIn,
				       G4bool                buildEndPiecesIn,
				       G4double              coilWidthFractionIn,
				       G4double              coilHeightFractionIn,
				       G4String              geometryTypeAndPathIn,
				       G4Colour*             colourIn):
  name(nameIn),
  geometryType(geometryTypeIn),
  horizontalWidth(horizontalWidthIn),
  outerMaterial(outerMaterialIn),
  innerRadius(innerRadiusIn),
  vhRatio(vhRatioIn),
  angleIn(angleInIn),
  angleOut(angleOutIn),
  yokeOnLeft(yokeOnLeftIn),
  hStyle(hStyleIn),
  buildEndPieces(buildEndPiecesIn),
  coilWidthFraction(coilWidthFractionIn),
  coilHeightFraction(coilHeightFractionIn),
  geometryTypeAndPath(geometryTypeAndPathIn),
  colour(colourIn)
{;}
  
std::ostream& operator<< (std::ostream& out, BDSMagnetOuterInfo const& info)
{
  out << "Magnet Outer Info:  \"" << info.name << "\""             << G4endl;
  out << "Geometry Type:       "  << info.geometryType             << G4endl;
  out << "Horizontal Width:    "  << info.horizontalWidth          << G4endl;
  out << "Material:            "  << info.outerMaterial->GetName() << G4endl;
  out << "Inner Radius:        "  << info.innerRadius              << G4endl;
  out << "V / H Ratio:         "  << info.vhRatio                  << G4endl;
  out << "Angle In:            "  << info.angleIn                  << G4endl;
  out << "Angle Out:           "  << info.angleOut                 << G4endl;
  out << "Yoke on Left:        "  << info.yokeOnLeft               << G4endl;
  out << "H Style (C if not):  "  << info.hStyle                   << G4endl;
  out << "Build end pieces:    "  << info.buildEndPieces           << G4endl;
  out << "Coil Width Fraction  "  << info.coilWidthFraction        << G4endl;
  out << "Coil Height Fraction "  << info.coilHeightFraction       << G4endl;
  out << "Geometry:            "  << info.geometryTypeAndPath      << G4endl;
  if (info.colour)
    {out << "Colour:              " << *(info.colour)             << G4endl;}
  else
    {out << "No colour specified" << G4endl;}
      
  return out;
}
