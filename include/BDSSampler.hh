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
#ifndef BDSSAMPLER_H
#define BDSSAMPLER_H

#include "BDSGeometryComponent.hh"

#include "globals.hh" // geant4 types / globals

class BDSApertureInfo;
class BDSBeamPipe;

/**
 * @brief Base class and registry of sampler instances.
 * 
 * @author Laurie Nevay
 */

class BDSSampler: public BDSGeometryComponent
{
public:
  explicit BDSSampler(G4String nameIn);
  virtual ~BDSSampler(){;}

  /// Return the name of this sampler.
  inline G4String GetName() const {return name;}
  
protected:
  /// Common construction tasks such as creating a logical volume from the solid
  /// and visualisation options.
  void CommonConstruction();

  /// Name of this sampler
  const G4String name;

private:
  /// Private default constructor to force the use of the supplied one.
  BDSSampler() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSSampler& operator=(const BDSSampler&) = delete;
  BDSSampler(BDSSampler&) = delete;
  /// @}
};

#endif
