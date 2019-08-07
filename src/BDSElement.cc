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
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSElement.hh"
#include "BDSException.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"

#include "globals.hh" // geant4 globals / types

#include <iomanip>

BDSElement::BDSElement(G4String nameIn,
		       G4double arcLengthIn,
		       G4double horizontalWidthIn,
		       G4String geometryIn,
		       G4double angleIn):
  BDSAcceleratorComponent(nameIn, arcLengthIn, angleIn, "element"),
  horizontalWidth(horizontalWidthIn),
  geometryFileName(geometryIn)
{;}

void BDSElement::BuildContainerLogicalVolume()
{
  // The horizontalWidth here is a suggested horizontalWidth for the factory. Each subfactory may treat this
  // differently.
  BDSGeometryExternal* geom = BDSGeometryFactory::Instance()->BuildGeometry(name, geometryFileName, nullptr,
									    chordLength, horizontalWidth);
  
  if (!geom)
    {throw BDSException(__METHOD_NAME__, "Error loading geometry in component \"" + name + "\"");}
  
  // We don't register the geometry as a daughter as the geometry factory retains
  // ownership of the geometry and will clean it up at the end.
  
  // make the beam pipe container, this object's container
  containerLogicalVolume = geom->GetContainerLogicalVolume();
  containerSolid         = geom->GetContainerSolid();

  // set placement offset from geom so it's placed correctly in the beam line
  SetPlacementOffset(geom->GetPlacementOffset());
  
  // update extents
  InheritExtents(geom);

  const BDSExtent geomExtent = geom->GetExtent();
  BDSExtent nominalExt = BDSExtent(horizontalWidth*0.5, horizontalWidth*0.5, chordLength*0.5);
  if (nominalExt.TransverselyGreaterThan(geomExtent))
    {SetExtent(nominalExt);}

  // check extent of geometry as compared to user input length of component in
  // beam line. If longer (including some numerical tolerance), warn user
  G4double extLength = GetExtent().DZ();
  G4double tolerance = 1*CLHEP::micrometer;
  if ((extLength - chordLength) > tolerance)
    {
      G4cerr << std::setprecision(15); // precise print out to aid user
      G4cerr.setf( std::ios::fixed, std:: ios::floatfield );
      G4cerr << "BDSElement> The loaded geometry is larger than the specified length"
	     << " of the element, which will cause overlaps!" << G4endl
	     << "Calculated extent along z of geometry: " << extLength << " mm" << G4endl;
      G4cerr << "Arc length    " << arcLength   << " mm"  << G4endl;
      G4cerr << "Bending angle " << angle       << " rad" << G4endl;
      G4cerr << "Chord length  " << chordLength << " mm"  << G4endl;
      G4cerr << "Chord length must be >= geometry length" << G4endl;
      G4cerr << "Possible overlaps in element \"" << name << "\"" << G4endl << G4endl << G4endl;
      // we don't force an exit here as our testing might not be exact for angled components
      // for now, we leave it to the user to ensure this is acceptable
      //throw BDSException(__METHOD_NAME__, "overlaps in element \"" + name + "\"");
    }
}
