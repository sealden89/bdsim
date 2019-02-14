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
#ifdef USE_GDML
#include "BDSAcceleratorModel.hh"
#include "BDSDebug.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactoryGDML.hh"
#include "BDSGeometryInspector.hh"
#include "BDSGDMLPreprocessor.hh"  // also only available with USE_GDML
#include "BDSGlobalConstants.hh"

#include "globals.hh"
#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

class G4VSolid;

BDSGeometryFactoryGDML::BDSGeometryFactoryGDML()
{;}

BDSGeometryExternal* BDSGeometryFactoryGDML::Build(G4String componentName,
						   G4String fileName,
						   std::map<G4String, G4Colour*>* mapping,
						   G4double /*suggestedLength*/,
						   G4double /*suggestedHorizontalWidth*/)
{
  CleanUp();

  // Compensate for G4GDMLParser deficiency in loading more than one file with similar names
  // in objects. Prepend all names with component name.
  G4String processedFile;
  if (BDSGlobalConstants::Instance()->PreprocessGDML())
    {processedFile = BDS::PreprocessGDML(fileName, componentName);}
  else
    {processedFile = fileName;}
  
  G4GDMLParser* parser = new G4GDMLParser();
  parser->Read(processedFile, /*validate=*/true);

  G4VPhysicalVolume* containerPV = parser->GetWorldVolume();
  G4LogicalVolume*   containerLV = containerPV->GetLogicalVolume();
  G4VSolid*       containerSolid = containerLV->GetSolid();
  G4ThreeVector gdmlWorldOrigin(0,0,0);
  if (containerPV->GetName() == "world_volume_lv_PV")
    {
      gdmlWorldOrigin = parser->GetPosition("PygdmlOrigin"); // TODO check if Pygdml geometry
      gdmlWorldOrigin[2] = 0.0;
    }

  // record all pvs and lvs used in this loaded geometry
  std::set<G4VPhysicalVolume*> pvsGDML;
  std::set<G4LogicalVolume*>   lvsGDML;
  GetAllLogicalAndPhysical(containerPV, pvsGDML, lvsGDML);

  auto visesGDML = ApplyColourMapping(lvsGDML, mapping);

  ApplyUserLimits(lvsGDML, BDSGlobalConstants::Instance()->DefaultUserLimits());

  /// Now overwrite container lv vis attributes
  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());

  std::pair<BDSExtent, BDSExtent> outerInner = BDS::DetermineExtents(containerSolid);
  
  BDSGeometryExternal* result = new BDSGeometryExternal(containerSolid, containerLV,
                                                        outerInner.first,  /*outer*/
                                                        outerInner.second, /*inner*/
                                                        gdmlWorldOrigin);
  result->RegisterLogicalVolume(lvsGDML);
  result->RegisterPhysicalVolume(pvsGDML);
  result->RegisterVisAttributes(visesGDML);

  delete parser;
  return result;
}

void BDSGeometryFactoryGDML::GetAllLogicalAndPhysical(const G4VPhysicalVolume*      volume,
						      std::set<G4VPhysicalVolume*>& pvsIn,
						      std::set<G4LogicalVolume*>&   lvsIn)
{
  const auto& lv = volume->GetLogicalVolume();
  lvsIn.insert(lv);
  for (G4int i = 0; i < lv->GetNoDaughters(); i++)
    {
      const auto& pv = lv->GetDaughter(i);
      pvsIn.insert(pv);
      GetAllLogicalAndPhysical(pv, pvsIn, lvsIn); // recurse into daughter
    }
}

void BDSGeometryFactoryGDML::ReplaceStringInFile(const G4String& fileName,
						 const G4String& outputFileName,
						 const G4String& key,
						 const G4String& replacement)
{
  // open input file in read mode
  std::ifstream ifs(fileName);

  // verify file open.
  if (!ifs.is_open())
    {
      G4cerr << __METHOD_NAME__ << "Cannot open file \"" << fileName << "\"" << G4endl;
      exit(1);
    }

  std::ofstream fout(outputFileName);

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Original file:  " << fileName       << G4endl;
  G4cout << __METHOD_NAME__ << "Temporary file: " << outputFileName << G4endl;
#endif

  int lenOfKey = key.size();
  
  // loop over and replace
  std::string buffer;
  while (std::getline(ifs, buffer))
    {// if we find key, replace it
      int f = buffer.find(key);    
      if (f != -1)
	{
	  std::string outputString = std::string(buffer);
	  outputString.replace(f, lenOfKey, replacement);
	  fout << outputString << "\n"; // getline strips \n
	}
      else // copy line to temp file as is
	{fout << buffer << "\n";}
    }

  // clean up
  ifs.close();
  fout.close();
}

#endif
