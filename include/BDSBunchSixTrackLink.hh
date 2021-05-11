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
#ifndef BDSBUNCHSIXTRACKLINK_H
#define BDSBUNCHSIXTRACKLINK_H 

#include "BDSBunch.hh"
#include "BDSParticleExternal.hh"

#include <vector>

class BDSParticleCoordsFull;

/**
 * @brief A bunch distribution that holds a bunch from Sixtrack Link.
 * 
 * @author Laurie Nevay
 */

class BDSBunchSixTrackLink: public BDSBunch
{
public: 
  BDSBunchSixTrackLink();
  virtual ~BDSBunchSixTrackLink();

  /// Get the next particle.
  virtual BDSParticleCoordsFull GetNextParticleLocal();

  /// Append particle to vector for tracking.
  void AddParticle(BDSParticleDefinition* particleDefinitionIn,
		   const BDSParticleCoordsFull& coordsIn,
		   int   externalParticleID,
		   int   externalParentID);
  
  void ClearParticles();

  inline size_t Size() const {return particles.size();}
  inline int    CurrentExternalParticleID() const {return currentExternalParticleID;}
  inline int    CurrentExternalParentID()   const {return currentExternalParentID;}
  
  void UpdateGeant4ParticleDefinition(G4int pdgID);
  
private:
  G4int currentIndex;
  G4int currentExternalParticleID;
  G4int currentExternalParentID;
  G4int size;         ///< Number of particles (1 counting).
  std::vector<BDSParticleExternal*> particles;
};
#endif
