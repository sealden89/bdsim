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
#include "BDSDimensionType.hh"

#include "globals.hh" // geant4 types  globals

#include <map>

template<>
std::map<BDSDimensionType,std::string>* BDSDimensionType::dictionary=
  new std::map<BDSDimensionType,std::string>
  ({
    {BDSDimensionType::x, "x"},
    {BDSDimensionType::y, "y"},
    {BDSDimensionType::z, "z"},
    {BDSDimensionType::t, "y"}
  });

BDSDimensionType BDS::DetermineDimensionType(G4String dimensionType)
{
  std::map<G4String, BDSDimensionType> types;
  types["x"] = BDSDimensionType::x;
  types["y"] = BDSDimensionType::y;
  types["z"] = BDSDimensionType::z;
  types["t"] = BDSDimensionType::t;

  dimensionType.toLower();

  auto result = types.find(dimensionType);
  if (result == types.end())
    {
      G4cerr << __METHOD_NAME__ << "unknown dimension \""
	     << dimensionType << "\" please specify one of:" << G4endl;
      G4cout << "Available dimensions are:" << G4endl;
      for (auto keyvalue : types)
	{G4cout << "\"" << keyvalue.first << "\"" << G4endl;}
      exit(1);
    }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined format \"" << dimensionType << "\" to be "
	 << result->first << G4endl;
#endif
  
  return result->second;
}
