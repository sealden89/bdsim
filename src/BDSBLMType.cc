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
#include "BDSBLMType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"

#include "globals.hh"

#include <map>

// dictionary for BDSBLMType
template<>
std::map<BDSBLMType, std::string>* BDSBLMType::dictionary =
  new std::map<BDSBLMType, std::string> ({
					  {BDSBLMType::cylinder, "cylinder"},
					  {BDSBLMType::cube,     "cube"},
					  {BDSBLMType::sphere,   "sphere"}
    });

BDSBLMType BDS::DetermineBLMType(G4String blmType)
{
  std::map<G4String, BDSBLMType> types;
  types["cylinder"] = BDSBLMType::cylinder;
  types["cube"]     = BDSBLMType::cube;
  types["sphere"]   = BDSBLMType::sphere;

  blmType.toLower();

  auto result = types.find(blmType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << "\"" << blmType << "\" is not a valid blmType" << G4endl;

      G4cout << "Available geometry types are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      throw BDSException(__METHOD_NAME__, "");
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined blm type to be " << result->second  << G4endl;
#endif
  return result->second;
}
