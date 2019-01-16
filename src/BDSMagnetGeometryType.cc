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
#include "BDSMagnetGeometryType.hh"
#include "BDSDebug.hh"

#include "globals.hh"

#include <map>

// dictionary for BDSMagnetGeometryType
template<>
std::map<BDSMagnetGeometryType, std::string>* BDSMagnetGeometryType::dictionary =
  new std::map<BDSMagnetGeometryType, std::string> ({
   {BDSMagnetGeometryType::none,          "none"},
   {BDSMagnetGeometryType::cylindrical,   "cylindrical"},
   {BDSMagnetGeometryType::polescircular, "polescircular"},
   {BDSMagnetGeometryType::polessquare,   "polessquare"},
   {BDSMagnetGeometryType::polesfacet,    "polesfacet"},
   {BDSMagnetGeometryType::polesfacetcrop,"polesfacetcrop"},
   {BDSMagnetGeometryType::lhcleft,       "lhcleft"},
   {BDSMagnetGeometryType::lhcright,      "lhcright"},
   {BDSMagnetGeometryType::external,      "external"}
});

BDSMagnetGeometryType BDS::DetermineMagnetGeometryType(G4String geometryType)
{
  // If it contains a colon ":" it must be external geometry format format:filepath
  if (geometryType.contains(":"))
    {return BDSMagnetGeometryType::external;}
  
  std::map<G4String, BDSMagnetGeometryType> types;
  types["none"]            = BDSMagnetGeometryType::none;
  types["cylindrical"]     = BDSMagnetGeometryType::cylindrical;
  types["polescircular"]   = BDSMagnetGeometryType::polescircular;
  types["polessquare"]     = BDSMagnetGeometryType::polessquare;
  types["polesfacet"]      = BDSMagnetGeometryType::polesfacet;
  types["polesfacetcrop"]  = BDSMagnetGeometryType::polesfacetcrop;
  types["lhcleft"]         = BDSMagnetGeometryType::lhcleft;
  types["lhcright"]        = BDSMagnetGeometryType::lhcright;
  types["format:filepath"] = BDSMagnetGeometryType::external;

  geometryType.toLower();
  
  auto result = types.find(geometryType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cout << __METHOD_NAME__ << "\"" << geometryType << "\" is not a valid geometry type" << G4endl;
      
      G4cout << "Available magnet geometry types are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " determined geometry type to be " << result->second << G4endl;
#endif
  return result->second;
}
