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
#ifndef BDSSDMANAGER_H
#define BDSSDMANAGER_H

#include "G4Version.hh"

class BDSEnergyCounterSD;
class BDSSamplerSD;
class BDSTerminatorSD;
class BDSVolumeExitSD;

#if G4VERSION_NUMBER < 1030
// In this case we use only the energy counter SD and return it
// as a base class pointer. Include header so casting works.
#include "BDSEnergyCounterSD.hh"
#endif

/**
 * @brief A singleton class that holds all required sensitive
 * detector class instances.  
 * 
 * Each sensitive detector class
 * need only be instantiated once and attached to the relevant
 * volume. This instantiates all necessary SDs and holds them.
 * 
 * @author Laurie Nevay
 */

class BDSSDManager
{
public:
  static BDSSDManager* Instance(); /// Singleton accessor.

  ~BDSSDManager();

  /// SD for samplers (plane type).
  inline BDSSamplerSD* GetSamplerPlaneSD() const {return samplerPlane;}

  /// SD for samplers (cylinder type).
  inline BDSSamplerSD* GetSamplerCylinderSD() const {return samplerCylinder;}

  /// SD for measuring turns around circular machine and terminating
  /// particles appropriately.
  inline BDSTerminatorSD* GetTerminatorSD() const {return terminator;}
  
  /// SD for general energy counter.
  inline BDSEnergyCounterSD* GetEnergyCounterSD() const {return eCounter;}

  /// SD for tunnel energy counter.
  inline BDSEnergyCounterSD* GetEnergyCounterTunnelSD() const {return tunnelECounter;}

  /// SD for energy deposition in the world volume.
  inline BDSEnergyCounterSD* GetEnergyCounterWorldSD() const {return worldECounter;}

  /// SD for world exit hits.
  inline BDSVolumeExitSD* GetWorldExitSD() const {return worldExit;}

#if G4VERSION_NUMBER > 1029
  /// SD for multiple SDs for world - energy loss and exit.
  inline G4VSensitiveDetector* GetWorldCompleteSD() const {return worldCompleteSD;}
#else
  /// SD for world energy loss as in Geant earlier than 4.10.3 we can only have
  /// one SD for each logical volume.
  inline G4VSensitiveDetector* GetWorldCompleteSD() const {return worldECounter;}
#endif

private:
  /// Private default constructor for singleton.
  BDSSDManager();

  BDSSDManager(const BDSSDManager&) = delete;
  BDSSDManager& operator=(const BDSSDManager&) = delete;
 
  static BDSSDManager* _instance;

  /// @{ SD instance.
  BDSSamplerSD*       samplerPlane;
  BDSSamplerSD*       samplerCylinder;
  BDSTerminatorSD*    terminator;
  BDSEnergyCounterSD* eCounter;
  BDSEnergyCounterSD* tunnelECounter;
  BDSEnergyCounterSD* worldECounter;
  BDSVolumeExitSD*    worldExit;
#if G4VERSION_NUMBER > 1029
  G4VSensitiveDetector* worldCompleteSD;
#endif
  /// @}
};

#endif
