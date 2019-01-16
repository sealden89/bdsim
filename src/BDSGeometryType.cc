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
#include "BDSGeometryType.hh"
#include "BDSDebug.hh"
#include "globals.hh"

#include <map>

// dictionary for BDSGeometryType
template<>
std::map<BDSGeometryType, std::string>* BDSGeometryType::dictionary =
  new std::map<BDSGeometryType, std::string> ({
   {BDSGeometryType::mokka,  "mokka"},
   {BDSGeometryType::gmad,   "gmad"},
   {BDSGeometryType::gdml,   "gdml"}
});	

BDSGeometryType BDS::DetermineGeometryType(G4String geometryType)
{
  std::map<G4String, BDSGeometryType> types;
  types["mokka"]  = BDSGeometryType::mokka;
  types["gmad"]   = BDSGeometryType::gmad;
  types["gdml"]   = BDSGeometryType::gdml;

  geometryType.toLower();

  auto result = types.find(geometryType);
  if ( result == types.end() )
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << " " << geometryType << " is not a valid geometry type" << G4endl;
      
      G4cout << "Available geometry types are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined geometry type to be " << result->second << G4endl;
#endif
  return result->second;
}

