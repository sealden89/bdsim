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
#ifndef BDSBENDBUILDER_H
#define BDSBENDBUILDER_H

#include "globals.hh" // geant4 globals / types
#include "G4String.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSIntegratorType.hh"

class BDSAcceleratorComponent;
class BDSIntegratorSet;
class BDSLine;
class BDSMagnet;
class BDSMagnetStrength;

namespace GMAD
{
  struct Element;
}

namespace BDS
{  
  /// This calculates and constructs a BDSLine* of BDSMagnet*. The bend is split
  /// into multiple wedges. If a small or zero poleface angle is specified, the
  /// end wedges will have faces angled as appropriate, the remaining wedges will
  /// re-use a single identical 'central' wedge several times. For a stronger angled
  /// poleface, the faces of each wedge fade in/out from the poleface to the cental
  /// wedge in the middle. Thin fringefield elements are placed at the beginning and
  /// end of the beamline if required.
  BDSAcceleratorComponent* BuildSBendLine(const G4String&         elementName,
					  const GMAD::Element*    element,
					  BDSMagnetStrength*      st,
					  const G4double          brho,
					  const BDSIntegratorSet* integratorSet,
					  const G4double&         incomingFaceAngle,
					  const G4double&         outgoingFaceAngle,
					  const G4bool&           buildFringeFields,
					  const GMAD::Element*    prevElement,
					  const GMAD::Element*    nextElement);
  
  /// Construct beamline for an rbend.  A line is returned with a single
  /// magnet as the main dipole, but can have fringefield magnets placed
  /// either end if specified.
  BDSLine* BuildRBendLine(const G4String&         elementName,
			  const GMAD::Element*    element,
			  const GMAD::Element*    prevElement,
			  const GMAD::Element*    nextElement,
			  const G4double          brho,
			  BDSMagnetStrength*      st,
			  const BDSIntegratorSet* integratorSet,
			  const G4double&         incomingFaceAngle,
			  const G4double&         outgoingFaceAngle,
			  const G4bool&           buildFringeFields);

  /// Utility function to calculate the number of segments an sbend should be split into.
  /// Based on aperture error tolerance - default is 1mm.
  G4int CalculateNSBendSegments(const G4double& length,
				const G4double& angle,
				const G4double incomingFaceAngle = 0,
				const G4double outgoingFaceAngle = 0,
				const G4double aperturePrecision = 1.0);

  /// Thin magnet for dipole fringe field.
  /// Is beampipe only, no outer magnet.
  BDSMagnet* BuildDipoleFringe(const GMAD::Element*     element,
			       G4double                 angleIn,
			       G4double                 angleOut,
			       G4String                 name,
			       const BDSMagnetStrength* st,
			       G4double                 brho,
			       const BDSIntegratorSet*  integratorSet,
				   BDSFieldType 			dipoleFieldType);

  /// Function to return a single sector bend section.
  BDSMagnet* BuildSingleSBend(const GMAD::Element*     element,
			      const G4String           name,
			      const G4double           arcLength,
			      const G4double           angle,
			      const G4double           angleIn,
			      const G4double           angleOut,
			      const BDSMagnetStrength* strength,
			      const G4double           brho,
			      const BDSIntegratorSet*  integratorSet,
			      const G4bool             yokeOnLeft);
  
  void UpdateSegmentAngles(const G4int    index,
			   const G4int    nSBends,
			   const G4double semiAngle,
			   const G4double incomingFaceAngle,
			   const G4double outgoingFaceAngle,
			   G4double&      segmentAngleIn,
			   G4double&      segmentAngleOut);

  BDSMagnetStrength* GetFringeMagnetStrength(const GMAD::Element* element,
                             const BDSMagnetStrength*  st,
                             const G4double            fringeAngle,
                             const G4double            e1,
                             const G4double            e2,
                             const G4double            fintx,
                             const G4bool              entranceOrExit);

  /// Function to get the integrator type. Test for finite K1 and returns
  /// dipole or dipolequadrupole integrator as appropriate.
  BDSIntegratorType GetDipoleIntegratorType(const BDSIntegratorSet* integratorSet,
									        const GMAD::Element*          element);
}

#endif
