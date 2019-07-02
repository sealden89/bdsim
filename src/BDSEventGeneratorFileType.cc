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
#include "BDSEventGeneratorFileType.hh"
#include "BDSDebug.hh"

#include "globals.hh"

#include <map>

// dictionary for BDSEventGeneratorFileType
template<>
std::map<BDSEventGeneratorFileType, std::string>* BDSEventGeneratorFileType::dictionary =
  new std::map<BDSEventGeneratorFileType, std::string> ({
      {BDSEventGeneratorFileType::hepmc2,       "hepmc2"},
      {BDSEventGeneratorFileType::hepmc3,       "hepmc3"},
      {BDSEventGeneratorFileType::hpe,          "hpe"},
      {BDSEventGeneratorFileType::root,         "root"},
      {BDSEventGeneratorFileType::treeroot,     "treeroot"},
      {BDSEventGeneratorFileType::lhef,         "lhef"},
});

BDSEventGeneratorFileType BDS::DetermineEventGeneratorFileType(G4String distrType)
{
  std::map<G4String, BDSEventGeneratorFileType> types;

  types["hepmc2"]       = BDSEventGeneratorFileType::hepmc2;
  types["hepmc3"]       = BDSEventGeneratorFileType::hepmc3;
  types["hpe"]          = BDSEventGeneratorFileType::hpe;
  types["root"]         = BDSEventGeneratorFileType::root;
  types["treeroot"]     = BDSEventGeneratorFileType::treeroot;
  types["lhef"]         = BDSEventGeneratorFileType::lhef;

  distrType.toLower();

  auto result = types.find(distrType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << distrType << " is not a valid event generator file format" << G4endl;

      G4cout << "Available formats are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined event generator file format to be " << result->second  << G4endl;
#endif
  return result->second;
}
