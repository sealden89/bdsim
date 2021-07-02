/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSException.hh"
#include "BDSScorerType.hh"

#include "globals.hh"

#include <map>
#include <string>

/// Dictionary for BDSScorerType.
template<>
std::map<BDSScorerType, std::string>* BDSScorerType::dictionary =
        new std::map<BDSScorerType, std::string> ({
	    {BDSScorerType::cellcharge,      "cellcharge"},
	    {BDSScorerType::cellcharge3d,    "cellcharge3d"},
	    {BDSScorerType::depositeddose,   "depositeddose"},
	    {BDSScorerType::depositeddose3d, "depositeddose3d"},
	    {BDSScorerType::depositedenergy, "depositedenergy"},
	    {BDSScorerType::depositedenergy, "depositedenergy3d"},
	    {BDSScorerType::population,      "population"},
	    {BDSScorerType::population3d,    "population3d"},
	    {BDSScorerType::cellflux,        "cellflux"},
	    {BDSScorerType::cellflux3d,      "cellflux3d"},
	    {BDSScorerType::cellfluxscaled,  "cellfluxscaled"},
	    {BDSScorerType::cellfluxscaled3d,"cellfluxscaled3d"},
        {BDSScorerType::cellfluxscaledperparticle,   "cellfluxscaledperparticle"},
	    {BDSScorerType::cellfluxscaledperparticle3d, "cellfluxscaledperparticle3d"},
        {BDSScorerType::cellflux4d,"cellflux4d"}
        });

BDSScorerType BDS::DetermineScorerType(G4String scorerType)
{
  std::map<G4String, BDSScorerType> types;
  types["cellcharge"]        = BDSScorerType::cellcharge;
  types["cellcharge3d"]      = BDSScorerType::cellcharge3d;
  types["depositeddose"]     = BDSScorerType::depositeddose;
  types["depositeddose3d"]   = BDSScorerType::depositeddose3d;
  types["depositedenergy"]   = BDSScorerType::depositedenergy;
  types["depositedenergy3d"] = BDSScorerType::depositedenergy3d;
  types["population"]        = BDSScorerType::population;
  types["population3d"]      = BDSScorerType::population3d;
  types["cellflux"]          = BDSScorerType::cellflux;
  types["cellflux3d"]        = BDSScorerType::cellflux3d;
  types["cellfluxscaled"]    = BDSScorerType::cellfluxscaled;
  types["cellfluxscaled3d"]  = BDSScorerType::cellfluxscaled3d;
  types["cellfluxscaledperparticle"]   = BDSScorerType::cellfluxscaledperparticle;
  types["cellfluxscaledperparticle3d"] = BDSScorerType::cellfluxscaledperparticle3d;
  types["cellflux4d"] = BDSScorerType::cellflux4d;
  
  scorerType.toLower();
  
  auto result = types.find(scorerType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cout << __METHOD_NAME__ << "\"" << scorerType << "\" is not a valid scorer type" << G4endl;
      G4cout << "Available scorer types are:" << G4endl;
      for (const auto& it : types)
        {G4cout << "\"" << it.first << "\"" << G4endl;}
      throw BDSException(__METHOD_NAME__, "error.");
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " determined scorer type to be " << result->second << G4endl;
#endif
  return result->second;
}
