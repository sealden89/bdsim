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
#include "BDSColours.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactoryBase.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4String.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include <map>
#include <vector>

BDSGeometryFactoryBase::BDSGeometryFactoryBase():
  checkOverlaps(BDSGlobalConstants::Instance()->CheckOverlaps())
{
  CleanUpBase();
}

BDSGeometryFactoryBase::~BDSGeometryFactoryBase()
{;}

std::vector<G4VisAttributes*> BDSGeometryFactoryBase::ApplyColourMapping(std::vector<G4LogicalVolume*>& lvsIn,
									 std::map<G4String, G4Colour*>* mapping)
{
  std::vector<G4VisAttributes*> visAttributes; // empty vector

  // no mapping, just return.
  if (!mapping)
    {return visAttributes;}
  
  if (mapping->size() == 1)
    {// only one colour for all - simpler
      G4VisAttributes* vis = new G4VisAttributes(*BDSColours::Instance()->GetColour("gdml"));
      vis->SetVisibility(true);
      visAttributes.push_back(vis);
      for (auto lv : lvsIn)
	{lv->SetVisAttributes(*vis);}
      return visAttributes;
    }

  // else iterate over all lvs and required vis attributes
  // prepare required vis attributes
  std::map<G4String, G4VisAttributes*> attMap;
  visAttributes.reserve(mapping->size()); // expand but don't initialise
  for (const auto& it : *mapping)
    {
      G4VisAttributes* vis = new G4VisAttributes(*(it.second));
      vis->SetVisibility(true);
      visAttributes.push_back(vis);
      attMap[it.first] = vis;
    }

  for (auto lv : lvsIn)
    {// iterate over all volumes
      const G4String& name = lv->GetName();
      for (const auto& it : attMap)
	{// iterate over all mappings to find first one that matches substring
	  if (name.contains(it.first))
	    {
	      lv->SetVisAttributes(it.second);
	      break;
	    }
	}
    }
  return visAttributes;
}

void BDSGeometryFactoryBase::ApplyUserLimits(const std::vector<G4LogicalVolume*>& lvsIn,
					     G4UserLimits* userLimits)
{
  for (auto& lv : lvsIn)
    {lv->SetUserLimits(userLimits);}
}

void BDSGeometryFactoryBase::CleanUp()
{
  CleanUpBase();
}

void BDSGeometryFactoryBase::CleanUpBase()
{
  xmin = 0;
  xmax = 0;
  ymin = 0;
  ymax = 0;
  zmin = 0;
  zmax = 0;

  rotations.clear();
  pvs.clear();
  lvs.clear();
  solids.clear();
  vises.clear();
}

void BDSGeometryFactoryBase::ExpandExtent(const BDSExtent& ext)
{
  xmin = std::min(ext.XNeg(), xmin);
  xmax = std::max(ext.XPos(), xmax);
  ymin = std::min(ext.YNeg(), ymin);
  ymax = std::max(ext.YPos(), ymax);
  zmin = std::min(ext.ZNeg(), zmin);
  zmax = std::max(ext.ZPos(), zmax);
}

void BDSGeometryFactoryBase::ExpandExtent(G4double x0, G4double rx,
					  G4double y0, G4double ry,
					  G4double z0, G4double rz)
{
  xmin = std::min(x0-rx, xmin);
  xmax = std::max(x0+rx, xmax);
  ymin = std::min(y0-ry, ymin);
  ymax = std::max(y0+ry, ymax);
  zmin = std::min(z0-rz, zmin);
  zmax = std::max(z0+rz, zmax);
}

void BDSGeometryFactoryBase::ExpandExtent(G4double x0, G4double xLow, G4double xHigh,
					  G4double y0, G4double yLow, G4double yHigh,
					  G4double z0, G4double zLow, G4double zHigh)
{
  xmin = std::min(x0+xLow,  xmin);
  xmax = std::max(x0+xHigh, xmax);
  ymin = std::min(y0+yLow,  ymin);
  ymax = std::max(y0+yHigh, ymax);
  zmin = std::min(z0+zLow,  zmin);
  zmax = std::max(z0+zHigh, zmax);
}
