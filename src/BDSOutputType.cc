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
#include "BDSOutputType.hh"

#include "globals.hh" // geant4 types  globals

#include <map>

template<>
std::map<BDSOutputType,std::string>* BDSOutputType::dictionary=
  new std::map<BDSOutputType,std::string>
  ({{BDSOutputType::none,"none"}
    ,{BDSOutputType::rootevent,"rootevent"}});

BDSOutputType BDS::DetermineOutputType(G4String outputType)
{
  std::map<G4String, BDSOutputType> types;
  types["none"]      = BDSOutputType::none;
  types["rootevent"] = BDSOutputType::rootevent;

  outputType.toLower();

  auto result = types.find(outputType);
  if (result == types.end())
    {
      G4cerr << __METHOD_NAME__ << "unknown output format \""
	     << outputType << "\" please specify one of:" << G4endl;
      G4cout << "Available output types are:" << G4endl;
      for (auto keyvalue : types)
	{G4cout << "\"" << keyvalue.first << "\"" << G4endl;}
      exit(1);
    }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined format \"" << outputType << "\" to be "
	 << result->first << G4endl;
#endif
  
  return result->second;
}
