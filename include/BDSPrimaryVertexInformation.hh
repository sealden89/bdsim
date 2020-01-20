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
#ifndef BDSPRIMARYVERTEXINFORMATION_H
#define BDSPRIMARYVERTEXINFORMATION_H

#include "BDSParticleCoordsFullGlobal.hh"

#include "globals.hh"
#include "G4VUserPrimaryVertexInformation.hh"

class BDSParticleDefinition;

/**
 * @brief Full set of coordinates for association with primary vertex.
 *
 * This stores a full set of coordinates in both the curvilinear and global
 * Cartesian coordinate frame that can be associated with the G4PrimaryVertex
 * for an event.
 * 
 * @author Laurie Nevay
 */

class BDSPrimaryVertexInformation: public G4VUserPrimaryVertexInformation
{
public:
  BDSPrimaryVertexInformation(const BDSParticleCoordsFullGlobal& primaryVertexIn,
			      const BDSParticleDefinition*       particle);
  BDSPrimaryVertexInformation(const BDSParticleCoordsFullGlobal& primaryVertexIn,
			      const G4double chargeIn,
			      const G4double rigidityIn,
			      const G4double massIn,
			      const G4int    pdgID,
			      const G4int    nElectronsIn = 0);
  virtual ~BDSPrimaryVertexInformation(){;}

  /// Required implementation by virtual base class.
  virtual void Print() const;

  BDSParticleCoordsFullGlobal primaryVertex; ///< Full set of coordinates.
  G4double charge;     ///< Effective charge of primary.
  G4double rigidity;   ///< Rigidity of particle at construction.
  G4double mass;       ///< Mass of particle - recorded as G4Vertex doesn't take into account electrons.
  G4int    pdgID;      ///< Particle Data Group ID number.
  G4int    nElectrons; ///< Number of electrons if partially stripped ion or atom.

private:
  BDSPrimaryVertexInformation() = delete;
};

#endif
