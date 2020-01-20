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
#include "BDSGeometryWriter.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#ifdef USE_GDML
#include "G4GDMLParser.hh"
#endif
#include "G4FileUtilities.hh"
#include "G4TransportationManager.hh"

class G4VPhysicalVolume;

BDSGeometryWriter::~BDSGeometryWriter(){;}

BDSGeometryWriter::BDSGeometryWriter()
{;}

void BDSGeometryWriter::ExportGeometry(G4String geometryType,
				       G4String geometryFileName)
{
  if (geometryType == "gdml")
    {
#ifdef USE_GDML
      WriteToGDML(geometryFileName);
#else
      G4cerr << "Not possible to write out " << geometryFileName << ", compile USE_GDML true" << G4endl;
      exit(1);
#endif
    }
}

#ifdef USE_GDML
void BDSGeometryWriter::WriteToGDML(G4String           outputFileName,
				    G4VPhysicalVolume* volume)
{
  if (!volume)
    {volume = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume();}
  // gdml parser throws exception if file exists. Delete file in that case
  if (BDS::FileExists(outputFileName))
    {
      G4FileUtilities fileUtilities;
      fileUtilities.DeleteFile(outputFileName, "");
    }
  G4GDMLParser parser;
  parser.Write(outputFileName, volume, true);
}
#endif
