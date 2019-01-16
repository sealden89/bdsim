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
#ifndef BDSGEOMETTRYINSPECTOR_H
#define BDSGEOMETTRYINSPECTOR_H

#include "globals.hh"
#include "G4CutTubs.hh"

#include <utility>

class BDSExtent;
class G4VSolid;

namespace BDS
{
  /// Utility function to inspect a solid and determine it's outer (and posssibly inner)
  /// extent by casting to it's solid class.  Delegates to below methods which may use this
  /// method themselves for recursion in booleans.
  std::pair<BDSExtent, BDSExtent> DetermineExtents(const G4VSolid* solid);

  /// Inspect a G4DisplacedSolid.
  std::pair<BDSExtent, BDSExtent> InspectDisplacedSolid(const G4VSolid* solidIn);

  /// Inpsect a G4SubtractionSolid. Convention: solid B is assumed to represent the
  /// inner extent of the solid.
  std::pair<BDSExtent, BDSExtent> InspectSubtractionSolid(const G4VSolid* solidIn);

  /// Inspect a G4IntersectionSolid.
  std::pair<BDSExtent, BDSExtent> InspectIntersectionSolid(const G4VSolid* solidIn);

  /// Inspect a G4UnionSolid.
  std::pair<BDSExtent, BDSExtent> InspectUnionSolid(const G4VSolid* solidIn);

  /// Inspect a G4Box.
  std::pair<BDSExtent, BDSExtent> InspectBox(const G4VSolid* solidIn);

  /// Inspect a G4Tubs.
  std::pair<BDSExtent, BDSExtent> InspectTubs(const G4VSolid* solidIn);

  /// Inpsect a G4CutTubs.
  std::pair<BDSExtent, BDSExtent> InspectCutTubs(const G4VSolid* solidv); 

  /**
   * @brief Wrapper class to expose useful protected method of G4CutTubs.
   * @author Laurie Nevay
   */
  class BDSCutTubsTemp: public G4CutTubs
  {
  public:
    explicit BDSCutTubsTemp(const G4CutTubs* solid):
      G4CutTubs(*solid){;}
    virtual ~BDSCutTubsTemp() {;}
    virtual void GetMaxMinZ(G4double& zmin, G4double& zmax) const {return G4CutTubs::GetMaxMinZ(zmin,zmax);};
  };
}

#endif
