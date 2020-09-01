/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactoryBase.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4String.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include <algorithm>
#include <map>
#include <set>
#include <vector>

BDSGeometryFactoryBase::BDSGeometryFactoryBase()
{
  CleanUpBase();
}

BDSGeometryFactoryBase::~BDSGeometryFactoryBase()
{;}

std::set<G4VisAttributes*> BDSGeometryFactoryBase::ApplyColourMapping(std::set<G4LogicalVolume*>&    lvsIn,
								      std::map<G4String, G4Colour*>* mapping,
								      G4bool                         autoColour)
{
  std::set<G4VisAttributes*> visAttributes; // empty set

  // no mapping, just return.
  if (!mapping && !autoColour)
    {return visAttributes;}

  if (mapping)
    {
      if (mapping->size() == 1)
        {// only one colour for all - simpler
          G4VisAttributes* vis = new G4VisAttributes(*BDSColours::Instance()->GetColour("gdml"));
          vis->SetVisibility(true);
          visAttributes.insert(vis);
          for (auto lv : lvsIn)
            {lv->SetVisAttributes(*vis);}
          return visAttributes;
        }

      // else iterate over all lvs and required vis attributes
      // prepare required vis attributes
      std::map<G4String, G4VisAttributes*> attMap;
      for (const auto& it : *mapping)
        {
          G4VisAttributes* vis = new G4VisAttributes(*(it.second));
          vis->SetVisibility(true);
          visAttributes.insert(vis);
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
    }
  
  if (autoColour)
    {
      for (auto lv : lvsIn)
        {
          const G4VisAttributes* existingVis = lv->GetVisAttributes();
          if (!existingVis)
            {
              G4Colour* c = BDSColourFromMaterial::Instance()->GetColour(lv->GetMaterial());
              G4VisAttributes* vis = new G4VisAttributes(*c);
              vis->SetVisibility(true);
              visAttributes.insert(vis);
              lv->SetVisAttributes(vis);
            }
        }
    }

  return visAttributes;
}

void BDSGeometryFactoryBase::ApplyUserLimits(const std::set<G4LogicalVolume*>& lvsIn,
					     G4UserLimits* userLimits)
{
  for (auto& lv : lvsIn)
    {lv->SetUserLimits(userLimits);}
}

void BDSGeometryFactoryBase::CleanUp()
{
  CleanUpBase();
}

G4String BDSGeometryFactoryBase:: PreprocessedName(const G4String& objectName,
						   const G4String& /*acceleratorComponentName*/) const
{return objectName;}

std::set<G4LogicalVolume*> BDSGeometryFactoryBase::GetVolumes(const std::set<G4LogicalVolume*>& allLVs,
							      std::vector<G4String>*            volumeNames,
							      G4bool                            preprocessFile,
							      const G4String&                   componentName) const
{
  if (!volumeNames)
    {return std::set<G4LogicalVolume*>();}
  
  std::vector<G4String> expectedVolumeNames;
  if (preprocessFile)
    {
      // transform the names to those the preprocessor would produce
      expectedVolumeNames.resize(volumeNames->size());
      std::transform(volumeNames->begin(),
		     volumeNames->end(),
		     expectedVolumeNames.begin(),
		     [&](const G4String& n){return PreprocessedName(n, componentName);});
    }
  else
    {expectedVolumeNames = *volumeNames;}
  
  std::set<G4LogicalVolume*> volsMatched;
  for (const auto& en : expectedVolumeNames)
    {
      for (auto lv : allLVs)
	{
	  if (lv->GetName() == en)
	    {volsMatched.insert(lv);}
	}
    }
  return volsMatched;
}

void BDSGeometryFactoryBase::CleanUpBase()
{
  FactoryBaseCleanUp();
  xmin = 0;
  xmax = 0;
  ymin = 0;
  ymax = 0;
  zmin = 0;
  zmax = 0;
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
