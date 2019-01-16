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
#include "BDSTunnelType.hh"
#include "BDSDebug.hh"
#include "globals.hh"

#include <map>

// dictionary for BDSTunnelType
template<>
std::map<BDSTunnelType, std::string>* BDSTunnelType::dictionary =
  new std::map<BDSTunnelType, std::string> ({
   {BDSTunnelType::circular,       "circular"},
   {BDSTunnelType::elliptical,     "elliptical"},
   {BDSTunnelType::square,         "square"},
   {BDSTunnelType::rectangular,    "rectangular"},
   {BDSTunnelType::rectaboveground,"rectaboveground"},
   {BDSTunnelType::ilc,            "ilc"}
});
      
BDSTunnelType BDS::DetermineTunnelType(G4String tunnelType)
{
  std::map<G4String, BDSTunnelType> types;
  types["circular"]        = BDSTunnelType::circular;
  types["elliptical"]      = BDSTunnelType::elliptical;
  types["square"]          = BDSTunnelType::square;
  types["rectangular"]     = BDSTunnelType::rectangular;
  types["ilc"]             = BDSTunnelType::ilc;
  types["rectaboveground"] = BDSTunnelType::rectaboveground;

  tunnelType.toLower();

  auto result = types.find(tunnelType);
  if (result == types.end() )
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << " " << tunnelType << " is not a valid tunnel type" << G4endl;

      G4cout << "Available tunnel types are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " determined tunnel type to be " << result->second << G4endl;
#endif
  return result->second;
}
