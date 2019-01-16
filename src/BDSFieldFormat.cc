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
#include "BDSFieldFormat.hh"
#include "BDSDebug.hh"
#include "globals.hh"

#include <map>
#include <string>

// dictionary for BDSFieldFormat for reflexivity
template<>
std::map<BDSFieldFormat, std::string>* BDSFieldFormat::dictionary =
  new std::map<BDSFieldFormat, std::string> ({
      {BDSFieldFormat::none,      "none"},
      {BDSFieldFormat::bdsim1d,   "bdsim1d"},
      {BDSFieldFormat::bdsim2d,   "bdsim2d"},
      {BDSFieldFormat::bdsim3d,   "bdsim3d"},
      {BDSFieldFormat::bdsim4d,   "bdsim4d"},
      {BDSFieldFormat::poisson2d, "poisson2d"},
      {BDSFieldFormat::poisson2dquad, "poisson2dquad"},
      {BDSFieldFormat::poisson2ddipole, "poisson2ddipole"}
});	

BDSFieldFormat BDS::DetermineFieldFormat(G4String bFormat)
{
  std::map<G4String, BDSFieldFormat> formats;
  formats["none"]          = BDSFieldFormat::none;
  formats["bdsim1d"]       = BDSFieldFormat::bdsim1d;
  formats["bdsim2d"]       = BDSFieldFormat::bdsim2d;
  formats["bdsim3d"]       = BDSFieldFormat::bdsim3d;
  formats["bdsim4d"]       = BDSFieldFormat::bdsim4d;
  formats["poisson2d"]     = BDSFieldFormat::poisson2d;
  formats["poisson2dquad"] = BDSFieldFormat::poisson2dquad;
  formats["poisson2ddipole"] = BDSFieldFormat::poisson2ddipole;
  
  bFormat.toLower();

  auto result = formats.find(bFormat);
  if (result == formats.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << "\"" << bFormat
	     << "\" is not a valid field format" << G4endl;
      G4cout << "Available field formats are:" << G4endl;
      for (auto it : formats)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined field format to be " << result->second << G4endl;
#endif
  return result->second;
}
