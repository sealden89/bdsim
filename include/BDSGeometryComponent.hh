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
#ifndef BDSGEOMETRYCOMPONENT_H
#define BDSGEOMETRYCOMPONENT_H

#include "BDSExtent.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"

#include <utility>              //for std::pair
#include <vector>

class G4UserLimits;
class G4VisAttributes;
class G4VPhysicalVolume;
class G4VSensitiveDetector;
class G4VSolid;
namespace CLHEP {
  class HepRotation;
}
typedef CLHEP::HepRotation G4RotationMatrix;

/**
 * @brief A generic geometry component for a bdsim model.
 * 
 * This class contains the minimum information required
 * for any geometrical object for a bdsim geant4 model.
 * 
 * It represents one complete geometrical object and all
 * the (c++) objects that compose it - therefore, once constructed,
 * a BDSGeometryComponent instance owns all the objects of its members.
 * 
 * Note, the container solid and container logical volume are automatically
 * registered by the constructor and do not need to be registered separately
 * like all other volumes. There are safeguards to prevent double registration
 * and the consequential seg faults.
 * 
 * @author Laurie Nevay
 */

class BDSGeometryComponent
{
public:
  BDSGeometryComponent(G4VSolid*         containerSolidIn,
		       G4LogicalVolume*  containerLVIn,
		       BDSExtent         extentIn            = BDSExtent(),
		       BDSExtent         innerExtentIn       = BDSExtent(),
		       G4ThreeVector     placementOffsetIn   = G4ThreeVector(0,0,0),
		       G4RotationMatrix* placementRotationIn = nullptr);
  
  /// Copy constructor (no copying of registered objects)
  BDSGeometryComponent(const BDSGeometryComponent& component);
  /// Assignment operator not used
  BDSGeometryComponent& operator=(const BDSGeometryComponent&) = delete;
  virtual ~BDSGeometryComponent();

  /// @{ Accessor - see member for more info
  inline G4String          GetName()   const {return containerLogicalVolume->GetName();}
  inline G4VSolid*         GetContainerSolid()         const {return containerSolid;}
  inline G4LogicalVolume*  GetContainerLogicalVolume() const {return containerLogicalVolume;}
  inline G4Transform3D     GetPlacementTransform()     const;
  inline G4ThreeVector     GetPlacementOffset()        const {return placementOffset;}
  inline G4RotationMatrix* GetPlacementRotation()      const {return placementRotation;}
  inline BDSExtent         GetExtent()                 const {return outerExtent;}
  inline BDSExtent         GetInnerExtent()            const {return innerExtent;}
  inline std::pair<G4double,G4double> GetExtentX()     const {return outerExtent.ExtentX();}
  inline std::pair<G4double,G4double> GetExtentY()     const {return outerExtent.ExtentY();}
  inline std::pair<G4double,G4double> GetExtentZ()     const {return outerExtent.ExtentZ();}
  inline std::pair<G4double,G4double> GetInnerExtentX() const {return innerExtent.ExtentX();}   
  inline std::pair<G4double,G4double> GetInnerExtentY() const {return innerExtent.ExtentY();}
  inline std::pair<G4double,G4double> GetInnerExtentZ() const {return innerExtent.ExtentZ();}
  inline std::vector<G4VPhysicalVolume*> GetAllPhysicalVolumes()  const {return allPhysicalVolumes;}
  inline std::vector<G4RotationMatrix*>  GetAllRotationMatrices() const {return allRotationMatrices;}
  inline std::vector<G4VisAttributes*>   GetAllVisAttributes()    const {return allVisAttributes;}
  inline std::vector<G4UserLimits*>      GetAllUserLimits()       const {return allUserLimits;}
  inline std::vector<BDSGeometryComponent*> GetAllDaughters()     const {return allDaughters;};
  inline std::vector<G4VSolid*>          GetAllSolids()           const {return allSolids;};
  /// @}
  
  /// Set the offset from 0,0,0 that the object should ideally be placed in its parent
  inline void SetPlacementOffset(const G4ThreeVector& offsetIn) {placementOffset = G4ThreeVector(offsetIn);}

  /// @{ Set extent
  inline void SetExtent(BDSExtent extIn)      {outerExtent = extIn;}
  inline void SetInnerExtent(BDSExtent extIn) {innerExtent = extIn;}
  /// @}
  
  /// Get the extent of the object in the positive direction in all dimensions
  G4ThreeVector GetExtentPositive() const {return outerExtent.ExtentPositive();}

  /// Get the extent of the object in the negative direction in all dimensions
  G4ThreeVector GetExtentNegative() const {return outerExtent.ExtentNegative();}

  /// Update the extents of this object with those of another object
  void InheritExtents(BDSGeometryComponent const * const anotherComponent);

  /// Register another geometry component as belonging to this one. This component will
  /// then own and delete it as necessary.
  void RegisterDaughter(BDSGeometryComponent* anotherComponent);
  
  /// Register a solid as belonging to this geometry component, which then becomes responsible
  /// for it. Note, the container solid given in the constructor is automatically registered.
  void RegisterSolid(G4VSolid* solid, G4bool internalCheck = false);

  /// Apply RegisterSolid() to a vector of solids
  void RegisterSolid(std::vector<G4VSolid*> soilds);
  
  /// Register a logical volume as belonging to this geometry component, which then becomes
  /// responsible for it. Note the container logical volume is automatically registered.
  void RegisterLogicalVolume(G4LogicalVolume* logicalVolume, G4bool internalCheck = false);

  /// Apply RegisterLogicalVolume() to a vector of logical volumes
  void RegisterLogicalVolume(std::vector<G4LogicalVolume*> logicalVolumes);

  /// Mark a volume as one that should be made sensitive using the read out geometry.  Note, if
  /// a volume is already sensitive with a specialised sensitive detector, it should NOT be
  /// registered using this method.
  /// This method will also check and ensure that the sensitive logical volume is an already
  /// registered logical volume with this geometry component.  
  void RegisterSensitiveVolume(G4LogicalVolume* sensitiveVolume);

  /// Apply RegisterSensitiveVolume(G4LogicalVolume* sensitiveVolume) to a vector of logical volumes
  void RegisterSensitiveVolume(std::vector<G4LogicalVolume*> sensitiveVolumes);

  /// Register a physical volume as belonging to this geometry component, which then becomes
  /// responsible for it.
  void RegisterPhysicalVolume(G4VPhysicalVolume* physicalVolume, G4bool internalCheck = false);

  /// Apply RegisterPhysicalVolume() to a vector of physical volumes
  void RegisterPhysicalVolume(std::vector<G4VPhysicalVolume*> physicalVolumes);

  /// Register a rotation matrix as belonging to this geometry component, which then becomes
  /// responsible for it.
  void RegisterRotationMatrix(G4RotationMatrix* rotationMatrix, G4bool internalCheck = false);

  /// Apply Register RotationMatrix() to a vector of rotation matrices
  void RegisterRotationMatrix(std::vector<G4RotationMatrix*> rotationMatrices);

  /// Register a visualisation attribute object as belonging to this geometry component, which
  /// then becomes responsible for it.
  void RegisterVisAttributes(G4VisAttributes* visAttribute, G4bool internalCheck = false);

  /// Apply RegisterVisAttribute() to a vector of visualisation attributes
  void RegisterVisAttributes(std::vector<G4VisAttributes*> visAttributes);

  /// Register a user limits object as belonging to this geometry component, which then
  /// becomes responsible for it
  void RegisterUserLimits(G4UserLimits* userLimit, G4bool internalCheck = false);

  /// Apply RegisterUserLimit to a vector of user limits.
  void RegisterUserLimits(std::vector<G4UserLimits*> userLimits);
  
  /// Utility method to copy all the logical & physical volumes plus rotation matrices from a
  /// BDSGeometryComponent instance to this one. Useful for example when prefabricated objects
  /// from factories are made to belong to an object.
  void InheritObjects(BDSGeometryComponent* component);

  /// Access all logical volumes belonging to this component
  std::vector<G4LogicalVolume*> GetAllLogicalVolumes() const;

  /// Return all logical volumes that should be used for biasing minus any that are excluded.
  virtual std::vector<G4LogicalVolume*> GetAllBiasingVolumes() const;
  
  /// Access all sensitive volumes belonging to this component
  virtual std::vector<G4LogicalVolume*> GetAllSensitiveVolumes() const;

  /// Make all logical volumes sensitive (flagged only) - does not attach an SD.
  inline void MakeAllVolumesSensitive() {RegisterSensitiveVolume(GetAllLogicalVolumes());}

  /// Attach a sensitive detector class to all registered sensitive volumes in this component.
  void SetSensitiveDetector(G4VSensitiveDetector* sd);

  /// Remove a particular logical volume from the logical volumes that will be
  /// returned for biasing.
  void ExcludeLogicalVolumeFromBiasing(G4LogicalVolume* lv);

protected:
  
  G4VSolid*                 containerSolid;
  G4LogicalVolume*          containerLogicalVolume;
  BDSExtent outerExtent;
  BDSExtent innerExtent;
  
  /// registry of all daughter geometry components
  std::vector<BDSGeometryComponent*> allDaughters;
  
  /// registry of all solids belonging to this component
  std::vector<G4VSolid*> allSolids;
  
  std::vector<G4LogicalVolume*> allLogicalVolumes;
  // we have to keep a registry of all logical volumes to be able to associate
  // information with them at construction time - for example S position - that
  // can't be stored in the Logical Volume class itself without modifying geant
  
  /// registry of all volumes that should be made sensitive
  std::vector<G4LogicalVolume*> allSensitiveVolumes;

  /// registry of all physical volumes belonging to this component
  std::vector<G4VPhysicalVolume*> allPhysicalVolumes;

  /// registry of all rotation matrices belonging to this component
  std::vector<G4RotationMatrix*> allRotationMatrices;

  /// registry of all visualisation attributes belonging to this component
  std::vector<G4VisAttributes*> allVisAttributes;

  /// registry of all user limits belonging to this component
  std::vector<G4UserLimits*>    allUserLimits;

  /// The offset to be applied BEFORE rotation when placed - used to account for
  /// any asymmetry the component may have.
  G4ThreeVector                 placementOffset;

  /// The rotation to be applied when placed - used to account for any required rotation
  /// of the geometry before placement such as being built along a different axis. This
  /// is defaulted to a nullptr so MUST be tested before use - this saves memory as every
  /// piece of geometry uses this class.
  G4RotationMatrix*             placementRotation;

  /// Volumes to use for biasing. By default this is nullptr and the accessor returns
  /// allLogicalVolumes. However, if ExcludeLogicalVolumeFromBiasing is used, the vector
  /// is copied and this pointer set to that vector. This is memory efficient solution.
  std::vector<G4LogicalVolume*>* allBiasingVolumes;
};

inline G4Transform3D BDSGeometryComponent::GetPlacementTransform() const
{
  if (!placementRotation)
    {return G4Transform3D(G4RotationMatrix(), placementOffset);}
  else
    {return G4Transform3D(*placementRotation, placementOffset);}
}

#endif
