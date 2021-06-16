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
#include "BDSColours.hh"
#include "BDSColourFromMaterial.hh"

#include "G4Colour.hh"
#include "G4Material.hh"
#include "G4PhysicsOrderedFreeVector.hh"
#include "G4String.hh"
#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <map>
#include <vector>

BDSColourFromMaterial* BDSColourFromMaterial::instance = nullptr;

BDSColourFromMaterial* BDSColourFromMaterial::Instance()
{
  if (!instance)
    {instance = new BDSColourFromMaterial();}
  return instance;
}

BDSColourFromMaterial::~BDSColourFromMaterial()
{
  instance = nullptr;
  delete generalDensity;
}

BDSColourFromMaterial::BDSColourFromMaterial()
{
  BDSColours* c = BDSColours::Instance();
  defines["air"]         = c->GetColour("air:240 240 240 0.05");
  defines["airbdsim"]    = defines["air"];
  defines["G4_AIR"]      = defines["air"];
  
  defines["boron"]       = c->GetColour("reallyreallydarkgrey");
  defines["G4_BRASS"]    = c->GetColour("LHCcoil");
  
  defines["carbon"]      = c->GetColour("reallyreallydarkgrey");
  defines["G4_C"]        = defines["carbon"];
  defines["G4_GRAPHITE"] = defines["carbon"];
  
  defines["chlorine"]    = c->GetColour("yellow");
  defines["G4_Cl"]       = defines["chlorine"];
  
  defines["concrete"]    = c->GetColour("tunnel");
  defines["lhcconcrete"] = defines["concrete"];
  defines["G4_CONCRETE"] = defines["concrete"];
  
  defines["copper"]      = c->GetColour("coil");
  defines["G4_Cu"]       = defines["copper"];
  
  defines["iron"]        = c->GetColour("iron");
  defines["G4_Fe"]       = defines["iron"];
  
  defines["kapton"]      = c->GetColour("kapton");
  defines["G4_KAPTON"]   = defines["kapton"];
  
  defines["lead"]        = c->GetColour("lead");
  defines["G4_Pb"]       = defines["lead"];
  
  defines["marble"]      = c->GetColour("marble:228 228 228 1.0");
  
  defines["stainlesssteel"] = c->GetColour("beampipe");
  defines["G4_STAINLESS-STEEL"] = defines["stainlesssteel"];
  
  defines["sulphur"]     = c->GetColour("yellow");
  
  defines["water"]       = c->GetColour("water:0 102 204 0.5");
  defines["G4_WATER"]    = defines["water"];
  
  std::vector<G4double> densities = {1e2,  1,   0.1,  0.01, 1e-4}; // high to low
  for (auto& v : densities)
    {v *= CLHEP::g / CLHEP::cm3;}
  std::vector<G4double> values    = {100,  120,  150,  180,  210};
  generalDensity = new G4PhysicsOrderedFreeVector(&densities[0], &values[0], values.size());
}

G4Colour* BDSColourFromMaterial::GetColour(const G4Material* material)
{
  G4String materialName = material->GetName();
  materialName.toLower();

  // strip off g4 so we don't have to define duplicates of everything
  std::string toErase = "g4_";
  size_t pos = materialName.find(toErase);
  if (pos != std::string::npos)
    {materialName.erase(pos, toErase.length());}

  auto search = defines.find(materialName);
  if (search != defines.end())
    {return search->second;}
  else
    {
      G4double alpha = 1;
      G4State state = material->GetState();
      switch (state)
	{
	case G4State::kStateGas:
	  {alpha = 0.05; break;}
	case G4State::kStateSolid:
	  {alpha = 1.0; break;}
	case G4State::kStateLiquid:
	  {alpha = 0.5; break;}
	default:
	  {alpha = 1.0; break;}
	}
      G4double density = material->GetDensity();
      G4double value   = generalDensity->Value(density);
      G4String vs      = G4String(std::to_string(value));
      G4Colour* result = BDSColours::Instance()->GetColour(materialName + ":" +
							   vs + " " +
							   vs + " " +
							   vs + " " +
							   std::to_string(alpha));
      defines[materialName] = result; // cache it as we'd always produce the same result
      return result;
    }
}

G4Colour* BDSColourFromMaterial::GetColourWithDefault(const G4Material* material,
                                                      G4Colour* defaultIn) const
{
  G4String materialName = material->GetName();
  materialName.toLower();
  
  // strip off g4 so we don't have to define duplicates of everything
  std::string toErase = "g4_";
  size_t pos = materialName.find(toErase);
  if (pos != std::string::npos)
  {materialName.erase(pos, toErase.length());}
  
  auto search = defines.find(materialName);
  if (search != defines.end())
  {return search->second;}
  else
  {return defaultIn;}
}
