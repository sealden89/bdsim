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
#include "BDSAcceleratorModel.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSCollimator.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"

#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"

#include <map>

BDSCollimator::BDSCollimator(G4String  nameIn,
			     G4double  lengthIn,
			     G4double  horizontalWidthIn,
			     G4String  typeIn,
			     G4double  xApertureIn,
			     G4double  yApertureIn,
			     G4double  xOutApertureIn,
			     G4double  yOutApertureIn,
			     G4String  collimatorMaterialIn,
			     G4String  vacuumMaterialIn,
			     G4Colour* colourIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, typeIn),
  horizontalWidth(horizontalWidthIn),
  xAperture(xApertureIn),
  yAperture(yApertureIn),
  xOutAperture(xOutApertureIn),
  yOutAperture(yOutApertureIn),
  collimatorMaterial(collimatorMaterialIn),
  vacuumMaterial(vacuumMaterialIn),
  colour(colourIn)
{
  if(horizontalWidth==0)
    {horizontalWidth = BDSGlobalConstants::Instance()->HorizontalWidth();}

  if ((xAperture > 0.5 * horizontalWidth) || (yAperture > 0.5 * horizontalWidth))
    {
      G4cerr << __METHOD_NAME__ << "half aperture bigger than width!" << G4endl;
      G4cerr << "Horizontal width is " << horizontalWidth << " mm for component named: \""
             << name << "\"" << G4endl;
      G4cerr << "x aperture " << xAperture << " mm, y aperture " << yAperture << " mm" << G4endl;
      exit(1);
    }

  if ((xOutAperture > 0.5 * horizontalWidth) || (yOutAperture > 0.5 * horizontalWidth))
    {
      G4cerr << __METHOD_NAME__ << "half aperture exit bigger than width!" << G4endl;
      G4cerr << "Horizontal width is " << horizontalWidth << " mm for component named: \""
             << name << "\"" << G4endl;
      G4cerr << "x aperture " << xOutAperture << " mm, y aperture " << yOutAperture << " mm" << G4endl;
      exit(1);
    }

  if (BDS::IsFinite(xOutAperture) && (xAperture <= 0))
    {
      G4cout << __METHOD_NAME__
             << "Warning - no entrance aperture set for collimator - exit aperture parameters will be ignored"
             << G4endl;
    }

  if (BDS::IsFinite(xOutAperture) && BDS::IsFinite(yOutAperture) && BDS::IsFinite(xAperture) &&
      BDS::IsFinite(yAperture))
    {
      if ((xOutAperture / yOutAperture) != (xAperture / yAperture))
        {
          G4cout << __METHOD_NAME__ << "Warning - X/Y half axes ratio at entrance and exit apertures are not equal"
                 << G4endl;
        }
    }

  if (collimatorMaterialIn == "")
    {
      G4cout << __METHOD_NAME__ << "Warning - no material set for collimator - using copper" << G4endl;
      collimatorMaterial = "G4_Cu";
    }

  collimatorSolid = nullptr;
  innerSolid      = nullptr;
  vacuumSolid     = nullptr;

  tapered = (BDS::IsFinite(xOutAperture) && BDS::IsFinite(yOutAperture));

  if (!colour)
    {colour = BDSColours::Instance()->GetColour("collimator");}
}

BDSCollimator::~BDSCollimator()
{;}

void BDSCollimator::BuildContainerLogicalVolume()
{
  containerSolid = new G4Box(name + "_container_solid",
			     horizontalWidth*0.5,
			     horizontalWidth*0.5,
			     chordLength*0.5);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
}

void BDSCollimator::Build()
{
  BDSAcceleratorComponent::Build(); // calls BuildContainer and sets limits and vis for container

  // Swap variables around if exit size is larger than entrance size
  // Rotation for tapered collimator (needed for tapered elliptical collimator)
  G4bool isOutLarger = ((xOutAperture > xAperture) && (yOutAperture > yAperture));
  G4RotationMatrix* colRotate;
  if (tapered && isOutLarger)
    {
      std::swap(xAperture,xOutAperture);
      std::swap(yAperture,yOutAperture);
      colRotate = new G4RotationMatrix;
      colRotate->rotateX(CLHEP::pi);
      RegisterRotationMatrix(colRotate);
    }
  else
    {colRotate = nullptr;}

  G4VSolid* outerSolid = new G4Box(name + "_outer_solid",
                                   horizontalWidth * 0.5 - lengthSafety,
                                   horizontalWidth * 0.5 - lengthSafety,
                                   chordLength * 0.5   - lengthSafety);
  RegisterSolid(outerSolid);

  G4bool buildVacuumAndAperture = (BDS::IsFinite(xAperture) && BDS::IsFinite(yAperture));

  // only do subtraction if aperture actually set
  if(buildVacuumAndAperture)
    {
      BuildInnerCollimator();

      collimatorSolid = new G4SubtractionSolid(name + "_collimator_solid", // name
                                               outerSolid,                 // solid 1
                                               innerSolid);                // minus solid 2
      RegisterSolid(collimatorSolid);
    }
  else
    {collimatorSolid = outerSolid;}

  G4Material* material = BDSMaterials::Instance()->GetMaterial(collimatorMaterial);
  G4LogicalVolume* collimatorLV = new G4LogicalVolume(collimatorSolid,          // solid
                                                      material,                 // material
                                                      name + "_collimator_lv"); // name

  // register it in a set of collimator logical volumes
  BDSAcceleratorModel::Instance()->VolumeSet("collimators")->insert(collimatorLV);
  
  G4VisAttributes* collimatorVisAttr = new G4VisAttributes(*colour);
  collimatorLV->SetVisAttributes(collimatorVisAttr);
  RegisterVisAttributes(collimatorVisAttr);

  // user limits - provided by BDSAcceleratorComponent
  collimatorLV->SetUserLimits(userLimits);

  // register with base class (BDSGeometryComponent)
  RegisterLogicalVolume(collimatorLV);
  if (sensitiveOuter)
    {RegisterSensitiveVolume(collimatorLV);}

  G4PVPlacement* collPV = new G4PVPlacement(colRotate,               // rotation
                                            (G4ThreeVector)0,        // position
                                            collimatorLV,            // its logical volume
                                            name + "_collimator_pv", // its name
                                            containerLogicalVolume,  // its mother  volume
                                            false,		     // no boolean operation
                                            0,		             // copy number
                                            checkOverlaps);

  RegisterPhysicalVolume(collPV);

  if (buildVacuumAndAperture)
    {
      G4Material* vMaterial = nullptr;

      if (vacuumMaterial.empty())
        {vMaterial = BDSGlobalConstants::Instance()->DefaultBeamPipeModel()->vacuumMaterial;}
      else
        {vMaterial = BDSMaterials::Instance()->GetMaterial(vacuumMaterial);}

      G4LogicalVolume *vacuumLV = new G4LogicalVolume(vacuumSolid,          // solid
                                                      vMaterial,            // material
                                                      name + "_vacuum_lv"); // name

      vacuumLV->SetVisAttributes(containerVisAttr);
      // user limits - provided by BDSAcceleratorComponent
      vacuumLV->SetUserLimits(userLimits);
      SetAcceleratorVacuumLogicalVolume(vacuumLV);
      RegisterLogicalVolume(vacuumLV);
      if (sensitiveVacuum)
	{RegisterSensitiveVolume(vacuumLV);}

      G4PVPlacement *vacPV = new G4PVPlacement(colRotate,               // rotation
                                               (G4ThreeVector) 0,       // position
                                               vacuumLV,                // its logical volume
                                               name + "_vacuum_pv",     // its name
                                               containerLogicalVolume,  // its mother  volume
                                               false,                   // no boolean operation
                                               0,                       // copy number
                                               checkOverlaps);

      RegisterPhysicalVolume(vacPV);
    }
}
