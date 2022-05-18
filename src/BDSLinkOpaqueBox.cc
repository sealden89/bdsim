/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#include "BDSAcceleratorComponent.hh"
#include "BDSApertureInfo.hh"
#include "BDSApertureType.hh"
#include "BDSBeamline.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSLinkOpaqueBox.hh"
#include "BDSMaterials.hh"
#include "BDSSamplerCustom.hh"
#include "BDSSamplerPlacementRecord.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSDManager.hh"
#include "BDSSDSamplerLink.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4TwoVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <limits>

BDSLinkOpaqueBox::BDSLinkOpaqueBox(BDSAcceleratorComponent* acceleratorComponentIn,
				   BDSTiltOffset* tiltOffsetIn,
				   G4double outputSamplerRadiusIn):
  BDSGeometryComponent(nullptr, nullptr),
  component(acceleratorComponentIn),
  outputSamplerRadius(outputSamplerRadiusIn),
  sampler(nullptr)
{
  if (tiltOffsetIn->HasFiniteTilt() && BDS::IsFinite(component->GetAngle()))
    {throw BDSException(__METHOD_NAME__, "finite tilt with angled component unsupported.");}

  G4double tilt = tiltOffsetIn->GetTilt();
  G4double ox   = tiltOffsetIn->GetXOffset();
  G4double oy   = tiltOffsetIn->GetYOffset();
  BDSExtent extent = component->GetExtent();
  extent = extent.TiltOffset(tiltOffsetIn);
  const G4double gap                = 10 * CLHEP::cm;
  const G4double opaqueBoxThickness = 10 * CLHEP::mm;
  G4String name = component->GetName();

  G4double mx = extent.MaximumX();
  G4double my = extent.MaximumY();
  G4double mr = std::max({mx, my, outputSamplerRadius});
  G4double mz = extent.MaximumZ();
  G4Box* terminatorBoxOuter = new G4Box(name + "_terminator_box_outer_solid",
					mr + gap + opaqueBoxThickness,
					mr + gap + opaqueBoxThickness,
					mz + gap + opaqueBoxThickness);
  RegisterSolid(terminatorBoxOuter);
  G4Box* terminatorBoxInner = new G4Box(name + "_terminator_box_inner_solid",
					mr + gap,
					mr + gap,
					mz + gap);
  RegisterSolid(terminatorBoxInner);
  G4SubtractionSolid* opaqueBox = new G4SubtractionSolid(name + "_opaque_box_solid",
							 terminatorBoxOuter,
							 terminatorBoxInner);
  RegisterSolid(opaqueBox);
  G4LogicalVolume* opaqueBoxLV = new G4LogicalVolume(opaqueBox,
						     BDSMaterials::Instance()->GetMaterial("G4_Galactic"),
						     name + "_opaque_box_lv");
  RegisterLogicalVolume(opaqueBoxLV);

  G4UserLimits* termUL = new G4UserLimits();
  termUL->SetUserMinEkine(std::numeric_limits<double>::max());
  RegisterUserLimits(termUL);
  opaqueBoxLV->SetUserLimits(termUL);
  
  G4VisAttributes* obVis = new G4VisAttributes(*BDSColours::Instance()->GetColour("opaquebox"));
  obVis->SetVisibility(true);
  opaqueBoxLV->SetVisAttributes(obVis);
  RegisterVisAttributes(obVis);
  
  G4double ls = BDSGlobalConstants::Instance()->LengthSafetyLarge();
  G4double margin = gap + opaqueBoxThickness + ls;
  G4double xsize = mr + margin;
  G4double ysize = mr + margin;
  G4double zsize = mz + margin;
  containerSolid = new G4Box(name + "_opaque_box_vacuum_solid",
			     xsize,
			     ysize,
			     zsize);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       BDSMaterials::Instance()->GetMaterial("G4_Galactic"),
					       name + "_container_lv");
  containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());

  // auto boxPlacement = 
  new G4PVPlacement(nullptr,
		    G4ThreeVector(),
		    opaqueBoxLV,
		    name + "_opaque_box_pv",
		    containerLogicalVolume,
		    false,
		    1,
		    true);

  G4RotationMatrix* rm = nullptr;
  if (BDS::IsFinite(tilt))
    {
      rm = new G4RotationMatrix();
      rm->rotateZ(tilt);
      RegisterRotationMatrix(rm);
    }
  // auto componentPlacement =
  G4cout << "Component placed at " << G4ThreeVector(ox,oy,0) << " inside opaque box" << G4endl;
  new G4PVPlacement(rm,
		    G4ThreeVector(ox,oy,0),
		    component->GetContainerLogicalVolume(),
		    component->GetName() + "_pv",
		    containerLogicalVolume,
		    false,
		    1,
		    true);
  
  outerExtent = BDSExtent(xsize, ysize, zsize);

  G4RotationMatrix* rm2 = new G4RotationMatrix();
  G4TwoVector xy = G4TwoVector(component->Sagitta(),0);
  if (BDS::IsFinite(tilt))
    {xy.rotate(tilt);}
  else if (BDS::IsFinite(component->GetAngle()))
    {rm2->rotate(-0.5 * component->GetAngle(), G4ThreeVector(0,1,0));}
  offsetToStart = G4ThreeVector(xy.x(), xy.y(), -0.5*component->GetChordLength());
  transformToStart = G4Transform3D(rm2->inverse(), offsetToStart);
  delete rm2;
  G4cout << "Transform to start (local) " << transformToStart.getTranslation() << G4endl;
}

BDSLinkOpaqueBox::~BDSLinkOpaqueBox()
{
  delete sampler;
}

G4int BDSLinkOpaqueBox::PlaceOutputSampler()
{  
  G4String samplerName = component->GetName() + "_out";
  BDSApertureType apt = BDSApertureType::circular;
  BDSApertureInfo ap = BDSApertureInfo(apt, outputSamplerRadius, 0, 0, 0);
  sampler = new BDSSamplerCustom(samplerName, ap);
  sampler->GetContainerLogicalVolume()->SetSensitiveDetector(BDSSDManager::Instance()->SamplerLink());
  
  auto z2 = component->GetExtent();
  G4ThreeVector position = G4ThreeVector(0,0,0.5*component->GetChordLength() + 2*BDSSamplerCustom::ChordLength());
  G4RotationMatrix* rm = nullptr;
  if (BDS::IsFinite(component->GetAngle()))
    {
      rm = new G4RotationMatrix();
      rm->rotateY(0.5 * component->GetAngle()); // rotate to output face
      RegisterRotationMatrix(rm);
      position = G4ThreeVector(component->Sagitta(), 0, 0.5*component->GetChordLength());
      G4ThreeVector gap = G4ThreeVector(0,0,2*BDSSamplerCustom::ChordLength());
      position += gap.transform(*rm);
    }
  // if there's finite angle, we ensure (in constructor) there's no tilt
  G4RotationMatrix* rml = rm ? new G4RotationMatrix(*rm) : new G4RotationMatrix();
  BDSSamplerPlacementRecord info(samplerName, sampler, G4Transform3D(*rml, position));
  delete rml;
  
  G4int samplerID = BDSSamplerRegistry::Instance()->RegisterSampler(info);
  new G4PVPlacement(rm,
		    position,
		    sampler->GetContainerLogicalVolume(),
		    samplerName + "_pv",
		    containerLogicalVolume,
		    false,
		    samplerID,
		    true);
  return samplerID;
}
