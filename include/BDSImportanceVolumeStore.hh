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
#ifndef BDSIMPORTANCEVOLUMESTORE_H
#define BDSIMPORTANCEVOLUMESTORE_H

#include "globals.hh"
#include "G4GeometryCell.hh"
#include "G4GeometryCellComp.hh"

#include <set>

class G4VPhysicalVolume;

typedef std::set<G4GeometryCell, G4GeometryCellComp> BDSSetGeometryCell;

/**
 * @brief Registry of importance values.
 * 
 * @author Will Shields
 */

class BDSImportanceVolumeStore
{
public:
  BDSImportanceVolumeStore();
  ~BDSImportanceVolumeStore();
  
  /// Add geometry cell to the store.
  void AddPVolume(const G4GeometryCell &cell);

  /// Get stores physical volume from index.
  const G4VPhysicalVolume* GetPVolume(G4int index) const;

  /// Length of this store. Useful for debugging.
  inline G4int size() const {return (G4int)fSetGeometryCell.size();}
  
private:
  BDSSetGeometryCell fSetGeometryCell;
};

#endif
