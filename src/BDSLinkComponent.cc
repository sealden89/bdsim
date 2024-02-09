/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSLinkComponent.hh"
#include "BDSLinkOpaqueBox.hh"

#include "G4Types.hh"
#include "G4ThreeVector.hh"


BDSLinkComponent::BDSLinkComponent(const G4String&      nameIn,
				   BDSLinkOpaqueBox*    componentIn,
				   G4double             arcLengthIn,
				   G4double             angleIn,
				   const G4ThreeVector& inputFaceNormalIn,
				   const G4ThreeVector& outputFaceNormalIn):
  BDSAcceleratorComponent(nameIn, arcLengthIn, angleIn, "linkComponent", nullptr,
			  inputFaceNormalIn, outputFaceNormalIn),
  component(componentIn)
{
  RegisterDaughter(component);
  InheritExtents(component);
  containerSolid = component->GetContainerSolid();
  containerLogicalVolume = component->GetContainerLogicalVolume();
}
