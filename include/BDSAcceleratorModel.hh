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
#ifndef BDSACCELERATORMODEL_H
#define BDSACCELERATORMODEL_H

#include "BDSBeamlineSet.hh"

#include "globals.hh"         // geant4 globals / types

#include <map>
#include <set>
#include <vector>

class BDSApertureInfo;
class BDSBeamline;
class BDSFieldObjects;
class G4LogicalVolume;
class G4ProductionCuts;
class G4Region;
class G4VPhysicalVolume;
class G4VSolid;

/**
 * @brief A holder class for all representations of the
 * accelerator model created in BDSIM. 
 *
 * This can be extended to 
 * allow inspection of the model. Holds the readout geometry
 * physical world in a location independent of detector 
 * construction.
 * 
 * In future, there may be several more beamlines than just
 * a flat one, and perhaps grouped into a more hierarchical
 * version.  These can be contained here and this class can
 * be extended as required.
 * 
 * @author Laurie Nevay
 */

class BDSAcceleratorModel
{
public:
  static BDSAcceleratorModel* Instance();
  ~BDSAcceleratorModel();

  /// @{ Register consituent of world.
  inline void RegisterWorldPV(G4VPhysicalVolume* worldIn) {worldPV = worldIn;}
  inline void RegisterWorldLV(G4LogicalVolume*   worldIn) {worldLV = worldIn;}
  inline void RegisterWorldSolid(G4VSolid*       worldIn) {worldSolid = worldIn;}
  /// @}
  
  /// Access the physical volume of the world
  inline G4VPhysicalVolume* WorldPV() const {return worldPV;}

  /// Register the main beam line set.
  void RegisterBeamlineSetMain(const BDSBeamlineSet& setIn);

  /// Register a set of beam lines to be managed and cleared up at the end of the simulation.
  void RegisterBeamlineSetExtra(G4String              name,
				const BDSBeamlineSet& setIn);
  
  /// @{ Accessor.
  inline const BDSBeamlineSet& BeamlineSetMain() const {return mainBeamlineSet;}
  const BDSBeamlineSet& BeamlineSet(G4String name) const;
  inline const std::map<G4String, BDSBeamlineSet>& ExtraBeamlines() const {return extraBeamlines;}
  const BDSBeamline* BeamlineMain() const {return mainBeamlineSet.massWorld;}
  /// @}

  /// Register the  beam line of arbitrary placements.
  inline void RegisterPlacementBeamline(BDSBeamline* placementBLIn) {placementBeamline = placementBLIn;}

  /// Access the beam line of arbitrary placements.
  inline BDSBeamline* PlacementBeamline() const {return placementBeamline;}
  
  /// Register the beam line containing all the tunnel segments
  inline void RegisterTunnelBeamline(BDSBeamline* beamlineIn) {tunnelBeamline = beamlineIn;}

  /// Access the beam line containing all the tunnel segments
  inline BDSBeamline* TunnelBeamline() const {return tunnelBeamline;}
  
  /// Register all field objects
  inline void RegisterFields(std::vector<BDSFieldObjects*>& fieldsIn){fields = fieldsIn;}

  /// Register a region and associated production cut as G4Region doesn't seem to delete
  /// it - note, no checking for double registration.
  void RegisterRegion(G4Region* region, G4ProductionCuts* cut);

  /// Register a single aperture.
  inline void RegisterAperture(G4String name, BDSApertureInfo* apertureIn) {apertures[name] = apertureIn;}
  
  /// Register a map of apertures with associated names.
  void RegisterApertures(const std::map<G4String, BDSApertureInfo*>& aperturesIn);

  /// Access an aperture definition. Will exit if not found. Note, we use pointers as
  /// we purposively don't provide a default constructor for BDSApertureInfo.
  BDSApertureInfo*  Aperture(G4String name) const;

  /// Access region information. Will exit if not found.
  G4Region*         Region(G4String name) const;

  /// Simpler accessor for production cuts vs regions.
  G4ProductionCuts* ProductionCuts(G4String name) {return cuts.at(name);}

  /// Returns pointer to a set of logical volumes. If no set by that name exits, create it.
  std::set<G4LogicalVolume*>* VolumeSet(G4String name);

  /// Check whether a volume is in a registry of volumes (a set). If no such registry exists
  /// then return false.
  G4bool VolumeInSet(G4LogicalVolume* volume,
		     G4String registryName);

  /// Find a corresponding mass world beam line for a curvilinear (or bridge) beam
  /// line from the registered beam line sets.
  BDSBeamline* CorrespondingMassWorldBeamline(BDSBeamline* bl) const;

  /// Return whether a beam line is a mass world beam line. If in the unlikely event the
  /// beam line isn't registered, false is returned by default.
  G4bool BeamlineIsMassWorld(BDSBeamline* bl) const;

  /// Update a beam line pointer and index if required for the equivalent ones in the mass
  /// world beam line. If the beam line supplied is a mass world one, nothing is done.
  void MassWorldBeamlineAndIndex(BDSBeamline*& bl,
				 G4int&        index) const;

private:
  BDSAcceleratorModel(); ///< Default constructor is private as singleton.

  static BDSAcceleratorModel* instance;

  G4VPhysicalVolume* worldPV;              ///< Physical volume of the mass world.
  G4LogicalVolume*   worldLV;
  G4VSolid*          worldSolid;

  BDSBeamlineSet mainBeamlineSet;
  std::map<G4String, BDSBeamlineSet> extraBeamlines; ///< Extra beamlines.

  /// Mapping from any curvilinear beam line to the corresponding mass world beam line.
  std::map<BDSBeamline*, BDSBeamline*> clToMassWorldMap;

  /// Map from beam line pointer to whether that beam line object is a mass world one,
  /// i.e. not a curvilinear or bridge one. 'TF' for true false.
  std::map<BDSBeamline*, G4bool> massWorldMapTF;

  /// Utility function to apply mapping.
  void MapBeamlineSet(const BDSBeamlineSet& setIn);

  BDSBeamline* tunnelBeamline;            ///< Tunnel segments beam line.
  BDSBeamline* placementBeamline;         ///< Placement beam line
  
  std::vector<BDSFieldObjects*>         fields;    ///< All field objects.
  std::map<G4String, G4Region*>         regions;   ///< All regions.
  std::map<G4String, BDSApertureInfo*>  apertures; ///< All apertures.
  std::map<G4String, G4ProductionCuts*> cuts;      ///< Cuts corresponding to the regions.

  std::map<G4String, std::set<G4LogicalVolume*>* > volumeRegistries; ///< All volume registries.
};

#endif
