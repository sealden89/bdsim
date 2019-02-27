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

#include "BDSSDType.hh"

#include "G4Version.hh"

#include <map>

class BDSSDCollimator;
class BDSSDEnergyDeposition;
class BDSSDEnergyDepositionGlobal;
class BDSMultiSensitiveDetectorOrdered;
class BDSSDSampler;
class BDSSDTerminator;
class BDSSDVolumeExit;

class G4VSDFilter;

#if G4VERSION_NUMBER < 1030
// In this case we use only the energy counter SD and return it
// as a base class pointer. Include header so casting works.
#include "BDSSDEnergyDeposition.hh"
#include "BDSSDEnergyDepositionGlobal.hh"
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

  /// Access a sensitive detector by the class enum. Default is a nullptr and also
  /// in the case of Geant < 4.10.3 for world complete as requires multiple sensitive
  /// detector. It's safe to set a nullptr to the SD of a logical volume. If applyOptions
  /// is used, the correct SD will only be returned if required accoring to the options
  /// in BDSGlobalConstants for storing hits. This way only hits are generated that are
  /// required (cpu and memory efficient).
  G4VSensitiveDetector* SensitiveDetector(const BDSSDType sdType,
					  G4bool applyOptions = false) const;

  /// SD for samplers (plane type).
  inline BDSSDSampler* SamplerPlane() const {return samplerPlane;}

  /// SD for samplers (cylinder type).
  inline BDSSDSampler* SamplerCylinder() const {return samplerCylinder;}

  /// SD for measuring turns around circular machine and terminating
  /// particles appropriately.
  inline BDSSDTerminator* Terminator() const {return terminator;}
  
  /// SD for general energy deposition.
  inline BDSSDEnergyDeposition* EnergyDeposition() const {return energyDeposition;}

  /// SD for general energy deposition but always include extra half of information.
  inline BDSSDEnergyDeposition* EnergyDepositionFull() const {return energyDepositionFull;}

  /// SD for energy deposition in vacuum volumes.
  inline BDSSDEnergyDeposition* EnergyDepositionVacuum() const {return energyDepositionVacuum;}

  /// SD for tunnel energy counter.
  inline BDSSDEnergyDeposition* EnergyDepositionTunnel() const {return energyDepositionTunnel;}

  /// SD for energy deposition in the world volume.
  inline BDSSDEnergyDepositionGlobal* EnergyDepositionWorld() const {return energyDepositionWorld;}

  /// SD for energy deposition in things that were already placed in the externally provided world.
  inline BDSSDEnergyDepositionGlobal* EnergyDepositionWorldContents() const {return energyDepositionWorldContents;}

  /// SD for world exit hits.
  inline BDSSDVolumeExit* WorldExit() const {return worldExit;}

#if G4VERSION_NUMBER > 1029
  /// SD for multiple SDs for world - energy loss and exit.
  inline G4VSensitiveDetector* WorldComplete() const {return worldCompleteSD;}
#else
  /// SD for world energy loss as in Geant earlier than 4.10.3 we can only have
  /// one SD for each logical volume.
  inline G4VSensitiveDetector* WorldComplete() const {return energyDepositionWorld;}
#endif

  /// SD for collimator impact locations.
  inline BDSSDCollimator* Collimator() const {return collimatorSD;}

  /// SD for collimator impacts + energy deposition at the same time in order.
  inline BDSMultiSensitiveDetectorOrdered* CollimatorComplete() const {return collimatorCompleteSD;}

private:
  /// Private default constructor for singleton.
  BDSSDManager();

  BDSSDManager(const BDSSDManager&) = delete;
  BDSSDManager& operator=(const BDSSDManager&) = delete;
 
  static BDSSDManager* instance;

  /// @{ SD instance.
  BDSSDSampler*                samplerPlane;
  BDSSDSampler*                samplerCylinder;
  BDSSDTerminator*             terminator;
  BDSSDEnergyDeposition*       energyDeposition;
  BDSSDEnergyDeposition*       energyDepositionFull;
  BDSSDEnergyDeposition*       energyDepositionVacuum;
  BDSSDEnergyDeposition*       energyDepositionTunnel;
  BDSSDEnergyDepositionGlobal* energyDepositionWorld;
  BDSSDEnergyDepositionGlobal* energyDepositionWorldContents;
  BDSSDVolumeExit*             worldExit;
#if G4VERSION_NUMBER > 1029
  G4VSensitiveDetector* worldCompleteSD;
#endif
  /// @}
  BDSSDCollimator* collimatorSD;
  BDSMultiSensitiveDetectorOrdered* collimatorCompleteSD;

  /// Map of all filters used. This class owns a single instance of each.
  std::map<G4String, G4VSDFilter*> filters;

  /// @{ Cache of global constant option.
  G4bool verbose;
  G4bool storeCollimatorHitsAll;
  G4bool storeCollimatorHitsIons;
  G4bool generateELossHits;
  G4bool generateELossVacuumHits;
  G4bool generateELossTunnelHits;
  G4bool generateELossWorldContents;
  G4bool storeELossWorld;
  G4bool storeELossExtras;
  G4bool generateCollimatorHits;
  /// @}
};

#endif
