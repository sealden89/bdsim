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
#ifndef BDSGEOMETRYFACTORY_H
#define BDSGEOMETRYFACTORY_H

#include "BDSGeometryType.hh"
#include "BDSSDType.hh"

#include "globals.hh"

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class BDSGeometryExternal;
class BDSGeometryFactoryBase;
class G4Colour;

/**
 * @brief Interface to external geometry construction.
 *
 * A factory that uses the correct derived singleton factory of
 * BDSGeometryFactoryBase to construct the required geometry with
 * extents.
 *
 * @author Lawrence Deacon, Laurie Nevay
 */

class BDSGeometryFactory
{
public:
  /// Singleton accessor.
  static BDSGeometryFactory* Instance();
  
  ~BDSGeometryFactory();

  /// Determine the geometry format, load it and build the geometry. Optional suggested
  /// length and horizontalWidth are in for some cases where it is not possible to query
  /// the geometry file for the extent information. Assign a default sensitivity to
  /// every volume recursively.
  BDSGeometryExternal* BuildGeometry(G4String componentName,
				     G4String formatAndFilePath,
				     std::map<G4String, G4Colour*>* colourMapping    = nullptr,
				     G4double               suggestedLength          = 0,
				     G4double               suggestedHorizontalWidth = 0,
				     std::vector<G4String>* namedVacuumVolumes       = nullptr,
				     G4bool                 makeSensitive            = true,
				     BDSSDType              sensitivityType          = BDSSDType::energydep);
 
private:
  /// Private accessor as singleton
  BDSGeometryFactory();

  /// Singleton instance.
  static BDSGeometryFactory* instance;

  /// A registry of all previously constructed components. We must use an
  /// std::string (which G4String inherits from) so we provide implicit hasher
  /// for the storage in the unordered map (which isn't provided for G4String).
  std::unordered_map<std::string, BDSGeometryExternal*> registry;

  /// This is where the geometry components are stored and used to manage
  /// the associated memory of the pieces of geometry.
  std::set<BDSGeometryExternal*> storage;

  /// Get the appropriate geometry factory
  BDSGeometryFactoryBase* GetAppropriateFactory(BDSGeometryType type);

  /// @{ Factory instance.
  BDSGeometryFactoryBase* gdml;
  BDSGeometryFactoryBase* gmad;
  BDSGeometryFactoryBase* sql;
  /// @}
};

#endif
