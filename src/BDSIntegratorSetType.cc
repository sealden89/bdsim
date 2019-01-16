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
#include "BDSIntegratorSetType.hh"

#include "G4Version.hh"

#include <map>
#include <string>

template<>
std::map<BDSIntegratorSetType, std::string>* BDSIntegratorSetType::dictionary =
  new std::map<BDSIntegratorSetType, std::string> ({
      {BDSIntegratorSetType::geant4,      "geant4"},
      {BDSIntegratorSetType::bdsimone,    "bdsimone"},
      {BDSIntegratorSetType::bdsimtwo,    "bdsimtwo"},
      {BDSIntegratorSetType::bdsimmatrix, "bdsimmatrix"},
      {BDSIntegratorSetType::bdsimmatrixfringescaling, "bdsimmatrixfringescaling"}
#if G4VERSION_NUMBER > 1039
      ,
      {BDSIntegratorSetType::geant4dp, "geant4dp"}
#endif
    });

BDSIntegratorSetType BDS::DetermineIntegratorSetType(G4String integratorSet)
{
  std::map<G4String, BDSIntegratorSetType> types;
  types["bdsim"]       = BDSIntegratorSetType::bdsimtwo; // alias for bdsim two the default
  types["geant4"]      = BDSIntegratorSetType::geant4;
  types["bdsimone"]    = BDSIntegratorSetType::bdsimone;
  types["bdsimtwo"]    = BDSIntegratorSetType::bdsimtwo;
  types["bdsimmatrix"] = BDSIntegratorSetType::bdsimmatrix;
  types["bdsimmatrixfringescaling"] = BDSIntegratorSetType::bdsimmatrixfringescaling;
#if G4VERSION_NUMBER > 1039
  types["geant4dp"]    = BDSIntegratorSetType::geant4dp;
#endif

  integratorSet.toLower();

  auto result = types.find(integratorSet);
  if (result == types.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << integratorSet << " is not a valid integrator set" << G4endl;

      G4cout << "Available sets are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined integrator set to be " << result->second << G4endl;
#endif
  return result->second;
}
