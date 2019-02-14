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
#ifndef BDSDETECTORCONSTRUCTION_H
#define BDSDETECTORCONSTRUCTION_H 

#include "BDSExtent.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"
#include "G4Version.hh"
#include "G4VUserDetectorConstruction.hh"

#include <list>
#include <string>
#include <vector>

class G4LogicalVolume;
class G4Region;
class G4VPhysicalVolume;

namespace GMAD {
  struct Element;
  template<typename T> class FastList;
  class Placement;
  class SamplerPlacement;
}

class BDSAcceleratorModel;
class BDSBeamline;
class BDSBeamlineSet;
class BDSComponentFactoryUser;
class BDSFieldObjects;
class BDSParticleDefinition;

#if G4VERSION_NUMBER > 1009
class BDSBOptrMultiParticleChangeCrossSection;
#endif

/**
 * @brief Class that constructs a Geant4 model of an accelerator.
 *
 * Mandatory class that must be supplied for a valid Geant4 simulation.
 * As construction is based on user input (via the parser) for a potentially
 * large number of items, the construction process is factorised into key steps
 * with member functions and makes use of factories for components and tunnel segments.
 * 
 * Maintained by Laurie Nevay & Jochem Snuverink
 */

class BDSDetectorConstruction: public G4VUserDetectorConstruction
{
public:
  explicit BDSDetectorConstruction(BDSComponentFactoryUser* userComponentFactoryIn = nullptr);
  virtual ~BDSDetectorConstruction();

  /// Loop over beam line and work out maximum tolerable sampler radius.
  void UpdateSamplerDiameter();

  /// Overridden Geant4 method that must be implemented. Constructs the Geant4 geometry
  /// and returns the finished world physical volume.
  virtual G4VPhysicalVolume* Construct();

  /// Construct sensitive detectors and fields.
  virtual void ConstructSDandField();

  /// Create biasing operations.
  void BuildPhysicsBias();

  /// Set the design particle definition.
  inline void SetDesignParticle(const BDSParticleDefinition* defIn) {designParticle = defIn;}

  /// Public access to the world extent.
  BDSExtent WorldExtent() const {return worldExtent;}

  /// Loop over a beam line and place elements in a container (world). If a sensitive
  /// detector is specified in each component, this is applied to each volume. If regions
  /// are desired,
  /// the element is looked up in the region definitions and that is set up. If
  /// registerInfo, physical volume info is created and placed in a pv info registry.
  /// Public and static so it can be used by parallel world constructors. Last argument
  /// is whether to use the placement transform for curvilinear coordinate geometry that's
  /// different in the case of tilted dipoles.
  static void PlaceBeamlineInWorld(BDSBeamline*          beamline,
				   G4VPhysicalVolume*    containerPV,
				   G4bool                checkOverlaps     = false,
				   G4bool                setRegions        = false,
				   G4bool                registerInfo      = false,
				   G4bool                useCLPlacementTransform = false);

  /// Create a transform based on the information in the placement.
  static G4Transform3D CreatePlacementTransform(const GMAD::Placement& placement,
						const BDSBeamline*     beamLine);

  /// Create a sampler placement. Turns the sampler placement into a placement and uses
  /// the above function.
  static G4Transform3D CreatePlacementTransform(const GMAD::SamplerPlacement& samplerPlacement,
						const BDSBeamline*            bemaline);
  
private:
  /// assignment and copy constructor not implemented nor used
  BDSDetectorConstruction& operator=(const BDSDetectorConstruction&);
  BDSDetectorConstruction(BDSDetectorConstruction&);

  /// Create and set parameters for various G4Regions
  void InitialiseRegions();

  /// Create all aperture definitions from parser and store in BDSAcceleratorModel.
  void InitialiseApertures();
  
  /// Build the main beam line and then any other required beam lines.
  void BuildBeamlines();

  /// Convert a parser beamline_list to BDSAcceleratorComponents with help of
  /// BDSComponentFactory and put in a BDSBeamline container that calculates coordinates
  /// and extents of the beamline.
  BDSBeamlineSet BuildBeamline(const GMAD::FastList<GMAD::Element>& beamLine,
			       G4String             name,
			       const G4Transform3D& initialTransform   = G4Transform3D(),
			       G4double             initialS           = 0.0,
			       G4bool               beamlineIsCircular = false);

  /// Build the tunnel around the already constructed flat beam line.
  void BuildTunnel();
  
  /// Build the world volume using the extent of the BDSBeamline instance created
  /// in BuildBeamline()
  G4VPhysicalVolume* BuildWorld();
  
  /// Place beam line, tunnel beam line, end pieces and placements in world.
  void ComponentPlacement(G4VPhysicalVolume* worldPV);

  /// Detect whether the first element has an angled face such that it might overlap
  /// with a previous element.  Only used in case of a circular machine.
  G4bool UnsuitableFirstElement(std::list<GMAD::Element>::const_iterator element);

#if G4VERSION_NUMBER > 1009
  /// Function that creates physics biasing cross section
  BDSBOptrMultiParticleChangeCrossSection* BuildCrossSectionBias(const std::list<std::string>& biasList,
								 G4String defaultBias,
								 G4String elementName);

  /// List of bias objects - for memory management
  std::vector<BDSBOptrMultiParticleChangeCrossSection*> biasObjects;
#endif

#ifdef BDSDEBUG
  bool debug = true;
#else
  bool debug = false;
#endif

  ///@{ Variable copied from global constants
  G4bool verbose;
  G4bool checkOverlaps;
  ///@}

  /// Accelerator model pointer
  BDSAcceleratorModel* acceleratorModel;

  /// All fields
  std::vector<BDSFieldObjects*> fields;

  G4bool       circular;    ///< Whether or not we're building a circular machine.
  BDSExtent    worldExtent; ///< Record of the world extent.
  BDSBeamline* placementBL; ///< Placement beam line.
  /// Particle definition all components are built w.r.t. Includes rigidity etc.
  const BDSParticleDefinition* designParticle;
  G4double     brho;        ///< Beam rigidity that accelerator will be constructed w.r.t.
  G4double     beta0;       ///< Beam relativistic beta that accelerator components use.
  G4bool canSampleAngledFaces; ///< Whether the integrator set permits sampling elements with angled faces.
  G4bool useExternalGeometryWorld;

  BDSComponentFactoryUser* userComponentFactory;
};

#endif

