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
#include "BDSImportanceVolumeStore.hh"

#include "globals.hh"
#include "G4VPhysicalVolume.hh"

#include <algorithm>
#include <set>

BDSImportanceVolumeStore::BDSImportanceVolumeStore(){;}

BDSImportanceVolumeStore::~BDSImportanceVolumeStore(){;}

void BDSImportanceVolumeStore::AddPVolume(const G4GeometryCell& cell)
{
  BDSSetGeometryCell::iterator it = fSetGeometryCell.find(cell);
  if (it != fSetGeometryCell.end())
    {
      G4cout << "BDSImportanceVolumeStore::AddPVolume: cell already stored" << G4endl;
      return;
    }
  fSetGeometryCell.insert(cell);
}


const G4VPhysicalVolume *BDSImportanceVolumeStore::GetPVolume(G4int index) const
{
  const G4GeometryCell& cell = *std::next(fSetGeometryCell.begin(), index);
  const G4VPhysicalVolume* pvol = &cell.GetPhysicalVolume();
  if (!pvol)
    {G4cout << "BDSImportanceVolumeStore::GetPVolume: no physical volume for cell: " << index << ", found" << G4endl;}
  return pvol;
}
