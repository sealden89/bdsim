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
#include "BDSCollimatorElliptical.hh"

#include "globals.hh" // geant4 globals / types
#include "G4EllipticalTube.hh"
#include "G4EllipticalCone.hh"
#include "G4VSolid.hh"

BDSCollimatorElliptical::BDSCollimatorElliptical(G4String    nameIn,
                                                 G4double    lengthIn,
                                                 G4double    horizontalWidthIn,
                                                 G4Material* collimatorMaterialIn,
                                                 G4Material* vacuumMaterialIn,
                                                 G4double    xApertureIn,
                                                 G4double    yApertureIn,
                                                 G4double    xApertureOutIn,
                                                 G4double    yApertureOutIn,
                                                 G4Colour*   colourIn):
  BDSCollimator(nameIn, lengthIn, horizontalWidthIn, "ecol",
                collimatorMaterialIn, vacuumMaterialIn,
                xApertureIn, yApertureIn, xApertureOutIn, yApertureOutIn, colourIn)
{;}

void BDSCollimatorElliptical::BuildInnerCollimator()
{
  if (tapered)
    {
      G4double zmax = chordLength * (xApertureOut + xAperture) / xAperture;

      G4double xhalf = 0.5 * (xApertureOut + xAperture);
      G4double yhalf = 0.5 * (yApertureOut + yAperture);

      innerSolid  = new G4EllipticalCone(name + "_inner_solid",    // name
                                         xhalf / zmax,             // Major axis of largest ellipse
                                         yhalf / zmax,             // Minor axis of largest ellipse
                                         zmax,                     // Height of cone
                                         zmax);   // Cut.
    
      vacuumSolid = new G4EllipticalCone(name + "_vacuum_solid",            // name
                                         xhalf/zmax- lengthSafety,          // Major axis of largest ellipse
                                         yhalf/zmax - lengthSafety,         // Minor axis of largest ellipse
                                         zmax,                              // Height of cone
                                         chordLength*0.5 - lengthSafety);   // Cut.
    }
  else
    {
      innerSolid  = new G4EllipticalTube(name + "_inner_solid",    // name
                                         xAperture,                // x half width
                                         yAperture,                // y half width
                                         chordLength);             // z half length
      // z half length long for unambiguous subtraction

      vacuumSolid = new G4EllipticalTube(name + "_inner_solid",    // name
                                         xAperture - lengthSafety, // x half width
                                         yAperture - lengthSafety, // y half width
                                         chordLength * 0.5);       // z half length
    }
        
  RegisterSolid(innerSolid);
  RegisterSolid(vacuumSolid);
}
