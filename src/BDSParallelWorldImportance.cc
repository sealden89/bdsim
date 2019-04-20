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
#include "BDSException.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSImportanceFileLoader.hh"
#include "BDSParallelWorldImportance.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4GeometryCell.hh"
#include "G4IStore.hh"
#include "G4LogicalVolume.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4SDParticleFilter.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSensitiveDetector.hh"

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

#include <string>
#include <fstream>

BDSParallelWorldImportance::BDSParallelWorldImportance(G4String name):
  G4VUserParallelWorld("importanceWorld_" + name),
  imWorldPV(nullptr)
{
  userLimits = BDSGlobalConstants::Instance()->DefaultUserLimits();
  imVolMap   = BDSGlobalConstants::Instance()->ImportanceVolumeMapFile();
  imGeomFile = BDSGlobalConstants::Instance()->ImportanceWorldGeometryFile();
  visAttr    = BDSGlobalConstants::Instance()->VisibleDebugVisAttr();
}

void BDSParallelWorldImportance::Construct()
{
  // load the cell importance values
  G4String importanceMapFile = BDS::GetFullPath(imVolMap);
  if (importanceMapFile.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSImportanceFileLoader<igzstream> loader;
      imVolumesAndValues = loader.Load(importanceMapFile);
#else
      throw BDSException(__METHOD_NAME__, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
    }
  else
    {
      BDSImportanceFileLoader<std::ifstream> loader;
      imVolumesAndValues = loader.Load(importanceMapFile);
    }

  // build world
  BuildWorld();
}

BDSParallelWorldImportance::~BDSParallelWorldImportance()
{;}

void BDSParallelWorldImportance::BuildWorld()
{
  // load the importance world geometry
  BDSGeometryExternal* geom = BDSGeometryFactory::Instance()->BuildGeometry("importanceWorld", imGeomFile, nullptr, 0, 0);

  // clone mass world for parallel world PV
  imWorldPV = GetWorld();

  G4LogicalVolume* worldLV = imWorldPV->GetLogicalVolume();

  // set parallel world vis attributes
  G4VisAttributes* samplerWorldVis = new G4VisAttributes(*(visAttr));
  samplerWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  worldLV->SetVisAttributes(samplerWorldVis);

  // set limits
  worldLV->SetUserLimits(userLimits);

  G4LogicalVolume* container = geom->GetContainerLogicalVolume();

  // Set motherLV for all daughters to be world LV, and add geometry cell
  for (G4int i = 0; i < container->GetNoDaughters(); i++)
    {
      G4VPhysicalVolume* daughter = container->GetDaughter(i);
      daughter->SetMotherLogical(worldLV);

      G4VPhysicalVolume* parallelPV = new G4PVPlacement(daughter->GetRotation(),
							daughter->GetTranslation(),
							daughter->GetLogicalVolume(),
							daughter->GetName(),
							worldLV,
							false,
							0);
      // second arg (0) is replica number - assume no replication of cells
      G4GeometryCell cell(*parallelPV, 0);
      imVolumeStore.AddPVolume(cell);
    }
}

G4VPhysicalVolume* BDSParallelWorldImportance::GetWorldVolume()
  {return imWorldPV;}


G4GeometryCell BDSParallelWorldImportance::GetGeometryCell(G4int i)
{
  const G4VPhysicalVolume* p = imVolumeStore.GetPVolume(i);
  if (p)
    {return G4GeometryCell(*p,0);}
  else
    {
      G4cout << __METHOD_NAME__ << " Couldn't get G4GeometryCell" << G4endl;
      return G4GeometryCell(*imWorldPV,-2);
    }
}

void BDSParallelWorldImportance::AddIStore()
{
  G4IStore* aIstore = G4IStore::GetInstance(imWorldPV->GetName());

  // create a geometry cell for the world volume replicaNumber is 0!
  G4GeometryCell gWorldVolumeCell(*imWorldPV, 0);

  // set world volume importance to 1
  aIstore->AddImportanceGeometryCell(1, gWorldVolumeCell);

  // set importance values and create scorers
  G4int numCells = (G4int) imVolumesAndValues.size();

  for (G4int cell = 0; cell < numCells; cell++)
    {
      G4GeometryCell gCell  = GetGeometryCell(cell);
      G4String cellName     = gCell.GetPhysicalVolume().GetName();
      G4double importanceValue = GetCellImportanceValue(cellName);
      
      //only add to store if it hasn't already been added. 0 is replica index.
      if (!aIstore->IsKnown(gCell))
	{aIstore->AddImportanceGeometryCell(importanceValue, gCell.GetPhysicalVolume(), 0);}
      else
	{
	  G4String message = "Geometry cell \"" + cellName + "\" already exists and has been previously\n";
	  message += "added to the IStore.";
	  throw BDSException(__METHOD_NAME__, message);
	}
    }
}

G4double BDSParallelWorldImportance::GetCellImportanceValue(G4String cellName)
{
  // prepare the user cellname for error message output.
  G4String finalCellName = cellName;
  // prependage and appendage added in pyg4ometry
  G4String preString1 = "importanceWorld_PREPEND";
  G4String preString2 = "importanceWorld_";
  G4String postString = "_pv";

  // if the cellname only contains the second prepend string, it frustratingly has to be replaced with the
  // first prepend as the first prepend string is automatically added to the map cell names when reading. This
  // was done to match the cell pv names written by pyg4ometry which previously included PREPEND when writing.
  // Have to check for PREPEND later anyway to maintain backwards compatibility.
  G4String newName = cellName;
  if (!cellName.contains(preString1) && cellName.contains(preString2))
    {newName = cellName.replace(0,preString2.size(),preString1);}

  // if the cellname is unmodified (e.g some other written geometry) then do nothing - up to
  // user to match names in map file

  auto result = imVolumesAndValues.find(newName);

  // only modify name if it contains one of the prestrings - we modify in pyg4ometry ("PREPEND"/"")
  // and this class (importanceWorld_), whereas the user will only know the name they defined.
  // can't check for poststring as G4 PV naming convention includes it.
  if (cellName.contains(preString1))
    {
      cellName = cellName.erase(0, preString1.size());
      finalCellName = cellName.erase(cellName.size() - postString.size(), postString.size());
    }

  if (result != imVolumesAndValues.end())
    {
      G4double importanceValue = (*result).second;
      // importance value must be finite and positive.
      if (importanceValue < 0)
        {
          G4String message = "Importance value is negative for cell \"" + finalCellName + "\".";
          throw BDSException(__METHOD_NAME__, message);
        }
      else if (!BDS::IsFinite(importanceValue))
        {
          G4String message = "Importance value is zero for cell \"" + finalCellName + "\".";
          throw BDSException(__METHOD_NAME__, message);
        }
      return importanceValue;
    }
  else
    {
      // exit if trying to get the importance value for a PV that isnt provided by the user.
      G4String message = "An importance value was not found for the cell \"" + finalCellName + "\" in \n";
      message += "the importance world geometry.";
      throw BDSException(__METHOD_NAME__, message);
    }
}

void BDSParallelWorldImportance::ConstructSD()
{
  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  // Sensitive Detector Name
  G4String importanceSamplingSDname = "ImportanceSamplingSD";

  // Create multifunctional detector for parallel world
  G4MultiFunctionalDetector* MFDet = new G4MultiFunctionalDetector(importanceSamplingSDname);
  SDman->AddNewDetector(MFDet); // Register SD to SDManager

  // Neutron filter
  G4SDParticleFilter* neutronFilter = new G4SDParticleFilter("neutronFilter", "neutron");
  MFDet->SetFilter(neutronFilter);

  //TODO: Add scorers here if necessary. Follow recipe in G4 example B03ImportanceDetectorConstruction.
}
