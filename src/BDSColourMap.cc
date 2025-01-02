/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSColourMap.hh"

#include "G4Colour.hh"
#include "G4String.hh"
#include "G4VisAttributes.hh"

#include <map>
#include <utility>

BDSColourMap::BDSColourMap()
{;}

BDSColourMap::BDSColourMap(const std::map<G4String, G4Colour*>& mapIn)
{
  MergeSimpleMap(mapIn);
}

BDSColourMap::~BDSColourMap()
{;}

void BDSColourMap::MergeSimpleMap(const std::map<G4String, G4Colour*>* mapIn,
                                  G4VisAttributes::ForcedDrawingStyle defaultStyle)
{
  if (mapIn)
    {MergeSimpleMap(*mapIn, defaultStyle);}
}

void BDSColourMap::MergeSimpleMap(const std::map<G4String, G4Colour*>& mapIn,
                                  G4VisAttributes::ForcedDrawingStyle defaultStyle)
{
  for (const auto& kv : mapIn)
    {data[kv.first] = std::make_pair(kv.second, defaultStyle);}
}

void BDSColourMap::SetForceDrawStyle(G4VisAttributes* vis,
                                     G4VisAttributes::ForcedDrawingStyle style)
{
  switch (style)
  {
    case G4VisAttributes::ForcedDrawingStyle::solid:
      {vis->SetForceSolid();}
    case G4VisAttributes::ForcedDrawingStyle::wireframe:
      {vis->SetForceWireframe();}
    case G4VisAttributes::ForcedDrawingStyle::cloud:
      {vis->SetForceCloud();}
  }
}