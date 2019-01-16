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
#ifndef BDSTRANSFORM3D_H
#define BDSTRANSFORM3D_H

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

/**
 * @brief Transform in beam line that takes up no space.
 *
 * This applies a transformation to cumulative curvilinear
 * coordinates as the beam line is built allowing an arbitrary
 * rotation or displacement of the reference coordinates from
 * that point on. A tilt or offset should be used instead of 
 * two of these in conjunction (sandwiching) to offset a particular
 * BDSAcceleratorComponent.
 *
 * @author Laurie Nevay
 */

class BDSTransform3D: public BDSAcceleratorComponent
{
public:
  BDSTransform3D(G4String aName, G4double x,
		 G4double y, G4double z, G4double phi,
		 G4double theta, G4double psi);

  virtual ~BDSTransform3D();

  ///@{ Access the change in reference coordinates this component should induce
  inline G4double GetDX() const {return dx;}
  inline G4double GetDY() const {return dy;}
  inline G4double GetDZ() const {return dz;}
  ///@}
  
  ///@{ Access the change in Euler angle this component should induce
  inline G4double GetDTheta() const {return dTheta;}
  inline G4double GetDPsi()   const {return dPsi;}
  inline G4double GetDPhi()   const {return dPhi;}
  ///@}
  
private:
  /// Private default constructor to force the use of the supplied one.
  BDSTransform3D() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSTransform3D& operator=(const BDSTransform3D&) = delete;
  BDSTransform3D(BDSTransform3D&) = delete;
  /// @}
  
  /// Contractual function overload from virtual base class
  /// BDSAcceleratorComponent. In this case has null implementation
  /// as the transform only inherits BDSAcceleratorComponent so that
  /// is compatible with the beamline
  virtual void BuildContainerLogicalVolume(){;};

  G4double dx;
  G4double dy;
  G4double dz;
  G4double dTheta;
  G4double dPsi;
  G4double dPhi;
};

#endif
