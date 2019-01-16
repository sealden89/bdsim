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
#include "BDSAcceleratorComponent.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSCollimatorCrystal.hh"
#include "BDSCrystalFactory.hh"
#include "BDSCrystalInfo.hh"
#include "BDSDebug.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSCollimatorCrystal::BDSCollimatorCrystal(G4String           nameIn, 
					   G4double           lengthIn,
					   BDSBeamPipeInfo*   beamPipeInfoIn,
					   BDSCrystalInfo*    crystalInfoLeftIn,
					   BDSCrystalInfo*    crystalInfoRightIn,
					   const G4double&    halfGapLeftIn,
					   const G4double&    halfGapRightIn,
					   const G4double&    angleYAxisLeftIn,
					   const G4double&    angleYAxisRightIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "drift", beamPipeInfoIn),
  crystalInfoLeft(crystalInfoLeftIn),
  crystalInfoRight(crystalInfoRightIn),
  halfGapLeft(halfGapLeftIn),
  halfGapRight(halfGapRightIn),
  angleYAxisLeft(angleYAxisLeftIn),
  angleYAxisRight(angleYAxisRightIn),
  crystalLeft(nullptr),
  crystalRight(nullptr)
{;}

BDSCollimatorCrystal::~BDSCollimatorCrystal()
{
  delete crystalInfoLeft;
  delete crystalInfoRight;
}

void BDSCollimatorCrystal::Build()
{
  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* pipe = factory->CreateBeamPipe(name,
					      chordLength,
					      beamPipeInfo);

  RegisterDaughter(pipe);
  
  // make the beam pipe container, this object's container
  containerLogicalVolume = pipe->GetContainerLogicalVolume();
  containerSolid         = pipe->GetContainerSolid();

  // register vacuum volume (for biasing)
  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());

  // update extents
  InheritExtents(pipe);

  // update faces
  SetInputFaceNormal(pipe->InputFaceNormal());
  SetOutputFaceNormal(pipe->OutputFaceNormal());

  BDSCrystalFactory* fac = new BDSCrystalFactory();
  if (crystalInfoLeft)
    {
      crystalLeft = fac->CreateCrystal(name + "_left", crystalInfoLeft);
      RegisterDaughter(crystalLeft);
      RegisterCrystalLVs(crystalLeft);
    }
  if (crystalInfoRight)
    {
      crystalRight = fac->CreateCrystal(name + "_right", crystalInfoRight);
      RegisterDaughter(crystalRight);
      RegisterCrystalLVs(crystalRight);
    }
  delete fac;

  if (crystalLeft)
    {
      G4ThreeVector objectOffset     = crystalLeft->GetPlacementOffset();
      G4ThreeVector colOffsetL       = G4ThreeVector(halfGapLeft,0,0);
      G4ThreeVector placementOffsetL = objectOffset + colOffsetL; // 'L' in p offset to avoid class with BDSGeometry Component member
      G4RotationMatrix* placementRot = crystalLeft->GetPlacementRotation();
      if (BDS::IsFinite(angleYAxisLeft))
	{
	  if (!placementRot)
	    {
	      placementRot = new G4RotationMatrix();
	      RegisterRotationMatrix(placementRot);
	    }
	  G4ThreeVector localUnitY = G4ThreeVector(0,1,0).transform(*placementRot);
	  placementRot->rotate(angleYAxisLeft, localUnitY); // rotate about local unitY
	}

      // check if it'll fit..
      BDSExtent extShifted = (crystalLeft->GetExtent()).Translate(placementOffsetL);
      BDSExtent thisExtent = GetExtent();
      G4bool safe = thisExtent.Encompasses(extShifted);
      if (!safe)
	{G4cout << __METHOD_NAME__ << "Left crystal potential overlap" << G4endl;}
      LongitudinalOverlap(crystalLeft->GetExtent(), angleYAxisLeft, "Left");
      
      auto cL = new G4PVPlacement(placementRot,
				  placementOffsetL,
				  crystalLeft->GetContainerLogicalVolume(),
				  name + "_crystal_left_pv",
				  GetAcceleratorVacuumLogicalVolume(),
				  false,
				  0,
				  true); // always check
      RegisterPhysicalVolume(cL);
    }
  if (crystalRight)
    {
      G4ThreeVector objectOffset     = crystalRight->GetPlacementOffset();
      G4ThreeVector colOffsetR       = G4ThreeVector(-halfGapRight,0,0); // -ve as r.h. coord system
      G4ThreeVector placementOffsetL = objectOffset + colOffsetR;
      G4RotationMatrix* placementRot = crystalRight->GetPlacementRotation();
      if (BDS::IsFinite(angleYAxisRight))
	{
	  if (!placementRot)
	    {
	      placementRot = new G4RotationMatrix();
	      RegisterRotationMatrix(placementRot);
	    }
	  G4ThreeVector localUnitY = G4ThreeVector(0,1,0).transform(*placementRot);
	  placementRot->rotate(angleYAxisRight, localUnitY); // rotate about local unitY
	}
      
      // check if it'll fit..
      BDSExtent extShifted = (crystalRight->GetExtent()).Translate(placementOffsetL);
      BDSExtent thisExtent = GetExtent();
      G4bool safe = thisExtent.Encompasses(extShifted);
      if (!safe)
	{G4cout << __METHOD_NAME__ << "Right crystal potential overlap" << G4endl;}
      LongitudinalOverlap(crystalRight->GetExtent(), angleYAxisLeft, "Right");

      auto cR = new G4PVPlacement(placementRot,
				  placementOffsetL,
				  crystalRight->GetContainerLogicalVolume(),
				  name + "_crystal_right_pv",
				  GetAcceleratorVacuumLogicalVolume(),
				  false,
				  0,
				  true); // always check
      RegisterPhysicalVolume(cR);
    }
  if (!crystalLeft && !crystalRight)
    {G4cout << __METHOD_NAME__ << "Error in crystal construction" << G4endl; exit(1);}
}

G4String BDSCollimatorCrystal::Material() const
{
  auto bpMat = GetBeamPipeInfo()->beamPipeMaterial;
  if (bpMat)
    {return bpMat->GetName();}
  else
    {return BDSAcceleratorComponent::Material();} // none
}

void BDSCollimatorCrystal::LongitudinalOverlap(const BDSExtent& extCrystal,
					       const G4double&  crystalAngle,
const G4String& side) const
{
  G4double zExt = extCrystal.MaximumZ();
  G4double dz   = zExt*std::tan(crystalAngle);

  G4bool overlap = 2*zExt + 2*std::abs(dz) > (chordLength - 2*lengthSafety);

  if (overlap)
    {
      G4cout << __METHOD_NAME__ << side << " crystal won't fit in collimator due to rotation." << G4endl;
      G4cout << "Crystal of length " << 2*zExt/CLHEP::mm << " mm is at angle "
	     << crystalAngle / CLHEP::mrad << " mrad and container is "
	     << chordLength/CLHEP::m << " m long." << G4endl;
      exit(1);
    }
}
 
void BDSCollimatorCrystal::RegisterCrystalLVs(const BDSCrystal* crystal) const
{
  auto crystals = BDSAcceleratorModel::Instance()->VolumeSet("crystals");
  auto collimators = BDSAcceleratorModel::Instance()->VolumeSet("collimators");
  for (auto lv : crystal->GetAllLogicalVolumes())
    {
      crystals->insert(lv);
      collimators->insert(lv);
    }
}
