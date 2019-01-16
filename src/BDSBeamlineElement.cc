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

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSDebug.hh"
#include "BDSExtentGlobal.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerType.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 globals / types
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include <ostream>

BDSBeamlineElement::BDSBeamlineElement(BDSAcceleratorComponent* componentIn,
				       G4ThreeVector            positionStartIn,
				       G4ThreeVector            positionMiddleIn,
				       G4ThreeVector            positionEndIn,
				       G4RotationMatrix*        rotationStartIn,
				       G4RotationMatrix*        rotationMiddleIn,
				       G4RotationMatrix*        rotationEndIn,
				       G4ThreeVector            referencePositionStartIn,
				       G4ThreeVector            referencePositionMiddleIn,
				       G4ThreeVector            referencePositionEndIn,
				       G4RotationMatrix*        referenceRotationStartIn,
				       G4RotationMatrix*        referenceRotationMiddleIn,
				       G4RotationMatrix*        referenceRotationEndIn,
				       G4double                 sPositionStartIn,
				       G4double                 sPositionMiddleIn,
				       G4double                 sPositionEndIn,
				       BDSTiltOffset*           tiltOffsetIn,
				       BDSSamplerType           samplerTypeIn,
				       G4String                 samplerNameIn,
				       G4int                    indexIn):
  component(componentIn),
  positionStart(positionStartIn), positionMiddle(positionMiddleIn), positionEnd(positionEndIn),
  rotationStart(rotationStartIn), rotationMiddle(rotationMiddleIn), rotationEnd(rotationEndIn),
  referencePositionStart(referencePositionStartIn),
  referencePositionMiddle(referencePositionMiddleIn),
  referencePositionEnd(referencePositionEndIn),
  referenceRotationStart(referenceRotationStartIn),
  referenceRotationMiddle(referenceRotationMiddleIn),
  referenceRotationEnd(referenceRotationEndIn),
  sPositionStart(sPositionStartIn), sPositionMiddle(sPositionMiddleIn), sPositionEnd(sPositionEndIn),
  tiltOffset(tiltOffsetIn),
  samplerType(samplerTypeIn),
  samplerName(samplerNameIn),
  samplerPlacementTransform(nullptr),
  index(indexIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__;
  if (componentIn)
    {G4cout << componentIn->GetName();}
  else
    {G4cerr << "WARNING - supplied component is invalid!" << G4endl;}
  G4cout << G4endl;
#endif

  componentIn->IncrementCopyNumber(); // increase copy number (starts at -1)
  copyNumber = componentIn->GetCopyNumber();
  /// placement name (starting at 0)
  placementName = componentIn->GetName() + "_" + std::to_string(copyNumber);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "unique placement name: \"" << placementName << "_pv\"" << G4endl;
#endif

  // create the placement transform from supplied rotation matrices and vector
  placementTransform   = new G4Transform3D(*rotationMiddle, positionMiddle);
  placementTransformCL = new G4Transform3D(*referenceRotationMiddle, referencePositionMiddle);

  // calculate sampler central position slightly away from end position of element.
  if (samplerType == BDSSamplerType::plane)
    {
      G4ThreeVector dZLocal = G4ThreeVector(0,0,1);
      // if we have a sampler, add on the thickness of the sampler to the padding
      dZLocal *= (BDSBeamline::PaddingLength() + BDSSamplerPlane::ChordLength())*0.5;
      dZLocal.transform(*referenceRotationEnd);
      G4ThreeVector samplerPosition = referencePositionEnd + dZLocal;
      samplerPlacementTransform = new G4Transform3D(*referenceRotationEnd, samplerPosition);
    }
  else if (samplerType == BDSSamplerType::cylinder)
    {samplerPlacementTransform = new G4Transform3D(*referenceRotationMiddle, referencePositionMiddle);}
}

BDSBeamlineElement::~BDSBeamlineElement()
{
  delete rotationStart;
  delete rotationMiddle;
  delete rotationEnd;
  delete referenceRotationStart;
  delete referenceRotationMiddle;
  delete referenceRotationEnd;
  delete placementTransform;
  delete placementTransformCL;
  delete samplerPlacementTransform;
}

BDSExtentGlobal BDSBeamlineElement::GetExtentGlobal() const
{
  BDSExtent ext = component->GetExtent();
  BDSExtentGlobal extG = BDSExtentGlobal(ext, *GetPlacementTransform());
  return extG;
}

G4ThreeVector BDSBeamlineElement::InputFaceNormal() const
{
  G4ThreeVector inputFNLocal = component->InputFaceNormal();
  if (!tiltOffset) // no tilt so the same as local
    {return inputFNLocal;}
  else
    {
      G4ThreeVector inputFNGlobal = inputFNLocal.rotateZ(tiltOffset->GetTilt());
      return inputFNGlobal;
    }
}

G4ThreeVector BDSBeamlineElement::OutputFaceNormal() const
{
  G4ThreeVector outputFNLocal = component->OutputFaceNormal();
  if (!tiltOffset) // no tilt so the same as local
    {return outputFNLocal;}
  else
    {
      G4ThreeVector outputFNGlobal = outputFNLocal.rotateZ(-tiltOffset->GetTilt());
      return outputFNGlobal;
    }
}

std::ostream& operator<< (std::ostream& out, BDSBeamlineElement const &e)
{
  out << "Beamline element: " << e.component->GetName() << G4endl;
  out << "Start, middle & end position: "
      << e.GetPositionStart()  << " " << e.GetPositionMiddle()  << " " << e.GetPositionEnd()  << G4endl
      << "Start, middle & end rotation: "
      << *(e.GetRotationStart())  << " " << *(e.GetRotationMiddle())  << " " << *(e.GetRotationEnd())  << G4endl
      << "Start, middle & end s position: "
      << e.GetSPositionStart() << " " << e.GetSPositionMiddle() << " " << e.GetSPositionEnd() << G4endl;

  return out;
}

G4bool BDSBeamlineElement::Overlaps(const BDSBeamlineElement* otherElement) const
{
  BDSExtentGlobal thisGlobal  = BDSExtentGlobal(component->GetExtent(),
						*(GetPlacementTransform()));
  BDSExtentGlobal otherGlobal = BDSExtentGlobal(otherElement->GetAcceleratorComponent()->GetExtent(),
						*(otherElement->GetPlacementTransform()));

  return thisGlobal.Overlaps(otherGlobal);
}


