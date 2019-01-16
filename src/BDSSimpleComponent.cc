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
#include "BDSGeometryComponent.hh"
#include "BDSSimpleComponent.hh"

#include "globals.hh" // geant4 types / globals


BDSSimpleComponent::BDSSimpleComponent(G4String              nameIn,
				       BDSGeometryComponent* component,
                                       G4double              arcLengthIn,
				       G4double              angleIn,
				       G4ThreeVector         inputFaceNormalIn,
				       G4ThreeVector         outputFaceNormalIn,
				       BDSBeamPipeInfo*      beamPipeInfoIn):
  BDSAcceleratorComponent(nameIn, arcLengthIn, angleIn, "simpleComponent", beamPipeInfoIn,
			  inputFaceNormalIn, outputFaceNormalIn)
{
  RegisterDaughter(component);
  InheritExtents(component);
  containerSolid = component->GetContainerSolid();
  containerLogicalVolume = component->GetContainerLogicalVolume();
}

BDSSimpleComponent::BDSSimpleComponent(G4String              nameIn,
				       G4double              arcLengthIn,
				       G4double              angleIn,
				       G4VSolid*             containerSolidIn,
				       G4LogicalVolume*      containerLogicalVolumeIn,
				       G4ThreeVector         inputFaceNormalIn,
				       G4ThreeVector         outputFaceNormalIn,
				       BDSBeamPipeInfo*      beamPipeInfoIn):
  BDSAcceleratorComponent(nameIn, arcLengthIn, angleIn, "simpleComponent", beamPipeInfoIn,
			  inputFaceNormalIn, outputFaceNormalIn)
{
  containerSolid         = containerSolidIn;
  containerLogicalVolume = containerLogicalVolumeIn;
}
