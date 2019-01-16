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
#include "BDSCavityType.hh"
#include "BDSDebug.hh"

#include <map>
#include <string>

// dictionary for BDSCavityType
template<>
std::map<BDSCavityType, std::string>* BDSCavityType::dictionary =
  new std::map<BDSCavityType, std::string> ({
  {BDSCavityType::elliptical,  "elliptical"},
  {BDSCavityType::rectangular, "rectangular"},
  {BDSCavityType::pillbox,     "pillbox"}
});

BDSCavityType BDS::DetermineCavityType(G4String cavityType)
{
  std::map<G4String, BDSCavityType> types;
  types["elliptical"]  = BDSCavityType::elliptical;
  types["rectangular"] = BDSCavityType::rectangular;
  types["pillbox"]     = BDSCavityType::pillbox;
  
  cavityType.toLower();

  auto result = types.find(cavityType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cout << __METHOD_NAME__ << "\"" << cavityType << "\" is not a valid cavity type" << G4endl;
      G4cout << "Available cavity types are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " determined cavity type to be " << result->second << G4endl;
#endif
  return result->second;
}
