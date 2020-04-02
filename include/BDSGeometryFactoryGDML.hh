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
#ifdef USE_GDML

#ifndef BDSGEOMETRYFACTORYGDML_H
#define BDSGEOMETRYFACTORYGDML_H

#include "BDSGeometryFactoryBase.hh"

#include "globals.hh"

#include <map>
#include <set>
#include <vector>

class BDSGeometryExternal;
class G4Colour;
class G4LogicalVolume;
class G4VPhysicalVolume;

/**
 * @brief Interface to Geant4's GDML loader.
 *
 * @author Stewart Boogert, Lawrence Deacon, Laurie Nevay
 */

class BDSGeometryFactoryGDML: public BDSGeometryFactoryBase
{
public:
  BDSGeometryFactoryGDML();
  virtual ~BDSGeometryFactoryGDML(){;}

  /// Main interface overrides base class method to construct a file with
  /// optional colour mapping.
  virtual BDSGeometryExternal* Build(G4String componentName,
				     G4String fileName,
				     std::map<G4String, G4Colour*>* colourMapping    = nullptr,
				     G4bool                 autoColour               = true,
				     G4double               suggestedLength          = 0,
				     G4double               suggestedHorizontalWidth = 0,
				     std::vector<G4String>* namedVacuumVolumes       = nullptr);

protected:
  /// Use the GDML preprocessing scheme to prepare the preprocesseed volume names.
  virtual G4String PreprocessedName(const G4String& objectName,
				    const G4String& acceleratorComponentName) const;
  
private:
  /// Create a temporary file in the current working directory (even though the geometry file
  /// may not exist there) and create a copy of the input geometry file line by line, but replacing
  /// a 'key' with 'replacement'. Returns the temporary file name created. Naming only allows one
  /// temporary file per component - so far, not a problem.
  void ReplaceStringInFile(const G4String& filename,
			   const G4String& outputFileName,
			   const G4String& key,
			   const G4String& replacement);

  /// Recursively append pvs and lvs from a given mother volume.  Pass by reference
  /// the output vectors
  void GetAllLogicalPhysicalAndMaterials(const G4VPhysicalVolume*         volume,
				                         std::set<G4VPhysicalVolume*>&    pvs,
				                         std::set<G4LogicalVolume*>&      lvs,
                                         std::map<G4String, G4Material*>& materialsGDML);
};

#endif

#endif
