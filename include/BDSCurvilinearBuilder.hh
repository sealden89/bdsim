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
#include "BDSBeamlineElement.hh"
#include "BDSUtilities.hh"

#include "globals.hh"

class BDSBeamline;
class BDSCurvilinearFactory;
class BDSSimpleComponent;

/**
 * @brief Factory for simple parallel geometry for curvilinear coordinates.
 *
 * This builds a beam line of geometry w.r.t. a beam line that can be used
 * for curvilinear coordinates.
 *
 * @author Laurie Nevay
 */

class BDSCurvilinearBuilder
{
public:
  BDSCurvilinearBuilder();
  ~BDSCurvilinearBuilder();

  /// Build a beam line of curvilinear geometry based on another beam line.
  BDSBeamline* BuildCurvilinearBeamLine1To1(BDSBeamline const* const beamline,
					    const G4bool circular);

  /// Build bridging volumes to join the curvilinear ones
  BDSBeamline* BuildCurvilinearBridgeBeamLine(BDSBeamline const* const beamline);

private:
  BDSCurvilinearBuilder(const BDSCurvilinearBuilder&) = delete;
  BDSCurvilinearBuilder& operator=(const BDSCurvilinearBuilder&) = delete;

  /// Create a curvilinear element for a beam line that represents the curvilinear
  /// coordinates between startElement and finishElement. This creates a BDSSimpleComponent
  /// first then wraps it in a premade BDSBeamlineElement. This must ONLY be used for a range
  /// of elements with the same tilt for the correct coordinate frame to be produced. The tilt
  /// is taken from the first element and assumed to be the same for all. The index is the
  /// index it'l have in the curvilinear beam line.
  BDSBeamlineElement* CreateCurvilinearElement(G4String                    elementName,
					       BDSBeamline::const_iterator startElement,
					       BDSBeamline::const_iterator finishElement,
					       G4int                       index);

  /// Create the BDSBeamlineElement by wrapping a BDSSimpleComponent.
  BDSBeamlineElement* CreateElementFromComponent(BDSSimpleComponent* component,
						 BDSBeamline::const_iterator startElement,
						 BDSBeamline::const_iterator finishElement,
						 G4int                       index);


  /// @{ Create a small section to extend a linear beam line.
  BDSBeamlineElement* CreateBonusSectionStart(BDSBeamline const* const beamline);
  BDSBeamlineElement* CreateBonusSectionEnd(BDSBeamline const* const beamline);
  /// @}

  /// Create a pre-made beam line element (because it's a non-continuous beam line) for
  /// a bridge section between two curvilinear volumes. Can cope if nextElement == end.
  /// numberOfUniqueComponents will be incremented if a new accelerator component is required.
  /// This is only done if the two components that are being bridged have angeld faces.
  /// beamlineIndex is the index the element will have in the non-continuous beam line;
  /// the indices should be continuous.
  BDSBeamlineElement* CreateBridgeSection(BDSAcceleratorComponent*    defaultBridge,
					  BDSBeamline::const_iterator element,
					  BDSBeamline::const_iterator nextElement,
					  BDSBeamline::const_iterator end,
					  G4int&                      numberOfUniqueComponents,
					  const G4int                 beamlineIndex);

  /// Create a single flat sided accelerator component for a small bridge volume. Intended
  /// to be reused as the default.
  BDSAcceleratorComponent* CreateDefaultBridgeComponent();

  /// Create a unique accelerator component for an element that's straight, but with a
  /// specific width.
  BDSAcceleratorComponent* CreateStraightBridgeComponent(G4double                    width,
							 G4int&                      numberOfUniqueComponents);
  
  /// Create a unique accelerator component for an element with angled faces.
  BDSAcceleratorComponent* CreateAngledBridgeComponent(BDSBeamline::const_iterator element,
						       G4int&                      numberOfUniqueComponents);

  /// Package an accelerator component into a beam line element w.r.t. two particular elements.
  /// Again, can cope with nextElement == end.
  BDSBeamlineElement* CreateBridgeElementFromComponent(BDSAcceleratorComponent*    component,
						       BDSBeamline::const_iterator element,
						       BDSBeamline::const_iterator nextElement,
						       BDSBeamline::const_iterator end,
						       const G4int                 beamlineIndex);

  /// Simple interrogation function to determine if an element has a finite angle or not.
  inline G4bool Angled(BDSBeamlineElement const* const element) const;

  /// Whether an element is too short for its own curvilinear volume.
  inline G4bool TooShort(BDSBeamlineElement const* const element) const;

  /// Length that was used for padding on the beam line we're building with respesct to.
  G4double paddingLength;

  G4double curvilinearRadius; ///< Radius for curvilinear geometry.
  G4double bonusChordLength;  ///< Length of any possible bonus section added to beginning and end.

  /// Factory to build curvilinear geometry.
  BDSCurvilinearFactory* factory;
};

inline G4bool BDSCurvilinearBuilder::Angled(BDSBeamlineElement const* const element) const
{return BDS::IsFinite(element->GetAngle());}
