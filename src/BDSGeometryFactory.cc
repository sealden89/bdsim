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
#include "BDSEnergyCounterSD.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGeometryFactoryBase.hh"
#ifdef USE_GDML
#include "BDSGeometryFactoryGDML.hh"
#endif
#include "BDSGeometryFactoryGMAD.hh"
#include "BDSGeometryFactorySQL.hh"
#include "BDSGeometryType.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals

#include <string>
#include <unordered_map>
#include <utility>

BDSGeometryFactory* BDSGeometryFactory::instance = nullptr;

BDSGeometryFactory* BDSGeometryFactory::Instance()
{
  if (!instance)
    {instance = new BDSGeometryFactory();}
  return instance;
}

BDSGeometryFactory::BDSGeometryFactory()
{
#ifdef USE_GDML
  gdml = new BDSGeometryFactoryGDML();
#else
  gdml = nullptr;
#endif
  gmad = new BDSGeometryFactoryGMAD();
  sql  = new BDSGeometryFactorySQL();
}

BDSGeometryFactory::~BDSGeometryFactory()
{
  delete gdml;
  delete gmad;
  delete sql;
  for (auto& geom : storage)
    {delete geom;}
  instance = nullptr;
}

BDSGeometryFactoryBase* BDSGeometryFactory::GetAppropriateFactory(BDSGeometryType type)
{
  switch(type.underlying())
    {
#ifdef USE_GDML
    case BDSGeometryType::gdml:
      {return gdml; break;}
#endif
    case BDSGeometryType::gmad:
      {return gmad; break;}
    case BDSGeometryType::mokka:
      {return sql; break;}
    default:
      {
	G4cout << "Unsupported factory type " << type;
	return nullptr;
	break;
      }
    }
}

BDSGeometryExternal* BDSGeometryFactory::BuildGeometry(G4String componentName,
						       G4String formatAndFileName,
						       std::map<G4String, G4Colour*>* colourMapping,
						       G4double suggestedLength,
						       G4double suggestedHorizontalWidth,
						       G4bool   makeSensitive)
{
  std::pair<G4String, G4String> ff = BDS::SplitOnColon(formatAndFileName);
  G4String fileName = BDS::GetFullPath(ff.second);

  const auto search = registry.find(fileName);
  if (search != registry.end())
    {return search->second;}// it was found already in registry
  // else wasn't found so continue

  // Check the file exists.
  if (!BDS::FileExists(fileName))
    {G4cerr << __METHOD_NAME__ << "No such file \"" << fileName << "\"" << G4endl; exit(1);}
  
  BDSGeometryType format = BDS::DetermineGeometryType(ff.first);
  BDSGeometryFactoryBase* factory = GetAppropriateFactory(format);
  if (!factory)
    {return nullptr;}
  
  BDSGeometryExternal* result = factory->Build(componentName, fileName, colourMapping,
					       suggestedLength, suggestedHorizontalWidth);
  
  if (result)
    {
      // Set all volumes to be sensitive.
      if (makeSensitive)
	{result->MakeAllVolumesSensitive();}
      
      registry[(std::string)fileName] = result;
      storage.push_back(result);
    }
  
  return result;
}
