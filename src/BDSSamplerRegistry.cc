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
#include "BDSSampler.hh"
#include "BDSSamplerRegistry.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

#include <map>
#include <string>
#include <vector>

BDSSamplerRegistry* BDSSamplerRegistry::instance = nullptr;

BDSSamplerRegistry* BDSSamplerRegistry::Instance()
{
  if (!instance)
    {instance = new BDSSamplerRegistry();}
  return instance;
}

BDSSamplerRegistry::BDSSamplerRegistry():
  numberOfEntries(0)
{;}

BDSSamplerRegistry::~BDSSamplerRegistry()
{
  instance = nullptr;
}

G4int BDSSamplerRegistry::RegisterSampler(G4String            name,
					  BDSSampler*         sampler,
					  G4Transform3D       transform,
					  G4double            S,
					  BDSBeamlineElement* element)
{
  G4String uniqueName = name;
  auto result = existingNames.find(name);
  if (result == existingNames.end())
    { // wan't found - introduce it
      existingNames[name] = 1;
    }
  else
    {
      uniqueName = name + "_" + std::to_string(existingNames[name]);
      existingNames[name]++;
    }
  BDSSamplerInfo info = BDSSamplerInfo(name, sampler, transform, S, element, uniqueName);
  return RegisterSampler(info);
}

G4int BDSSamplerRegistry::RegisterSampler(BDSSamplerInfo& info)
{
  infos.push_back(info);

  G4int index = numberOfEntries; // copy the number of entires / the index of this entry
  numberOfEntries++;
  return index;
}

std::vector<G4String> BDSSamplerRegistry::GetNames() const
{
  std::vector<G4String> names;
  for (auto info : infos)
    {names.push_back(info.Name());}
  return names;
}

std::vector<G4String> BDSSamplerRegistry::GetUniqueNames() const
{
  std::vector<G4String> names;
  for (auto info : infos)
    {names.push_back(info.UniqueName());}
  return names;
}
