/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#ifndef BDSSAMPLERINFO_H
#define BDSSAMPLERINFO_H

#include "BDSBeamlineElement.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

class BDSSampler;

/**
 * @brief Information about a registered sampler.
 * 
 * This class contains the information about a constructed
 * and placed sampler. The sampler registry holds a vector
 * of this class. This class is NOT a template to build a
 * sampler, but a record of what was built (unlike the other
 * BDSXXXXInfo classes in BDSIM.
 * 
 * This does not own the BDSSampler instance, merely keeps a 
 * record of it. Note, there can be many placements and therefore
 * registrations of one sampler instance. This also means that the
 * compiler provided default copy constructor is safe.
 * 
 * @author Laurie Nevay
 */

class BDSSamplerInfo
{
public:
  BDSSamplerInfo(const G4String&           nameIn,
		 BDSSampler*               samplerIn,
		 const G4Transform3D&      transformIn,
		 G4double                  sPositionIn  = -1000,
		 const BDSBeamlineElement* elementIn    = nullptr,
		 const G4String&           uniqueNameIn = "");

  ~BDSSamplerInfo(){;}

  /// @{ Accessor
  inline G4String      Name()             const {return name;}
  inline G4String      UniqueName()       const {return uniqueName;}
  inline BDSSampler*   Sampler()          const {return sampler;}
  inline G4Transform3D Transform()        const {return transform;}
  inline G4Transform3D TransformInverse() const {return transformInverse;}
  inline G4double      SPosition()        const {return sPosition;}
  inline const BDSBeamlineElement* Element() const {return element;}
  inline G4int         BeamlineIndex()    const;
  /// @}
  
private:
  /// Private default constructor to force use of provided one.
  BDSSamplerInfo();

  /// The name of the sampler
  G4String name;
  
  /// The sampler instance that this information pertains to.
  BDSSampler* sampler;

  /// The transform the sampler was placed with.
  G4Transform3D transform;

  /// The calculated inverse of the transform the sampler was placed with.
  G4Transform3D transformInverse;

  /// The curvilinear S position of this sampler.
  G4double      sPosition;

  /// The beam line element to which this sampler pertains (if any).
  const BDSBeamlineElement* element;

  /// The name of the sampler but suffixed with a number - guaranteed to be unique.
  G4String uniqueName;

};

G4int BDSSamplerInfo::BeamlineIndex() const
{
  if (element)
    {return element->GetIndex();}
  else
    {return -1;}
}

#endif
