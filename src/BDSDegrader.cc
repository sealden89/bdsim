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
#include "BDSDebug.hh"
#include "BDSDegrader.hh"
#include "BDSSDType.hh"

#include "G4Box.hh"
#include "G4ExtrudedSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include "globals.hh" // geant4 globals / types
#include <vector>

BDSDegrader::BDSDegrader(G4String   nameIn, 
			 G4double   lengthIn,
			 G4double   horizontalWidthIn,
			 G4int      numberWedgesIn,
			 G4double   wedgeLengthIn,
			 G4double   degraderHeightIn,
			 G4double   degraderOffsetIn,
			 G4Material* materialIn,
			 G4Colour*   colourIn):
  BDSAcceleratorComponent(nameIn, lengthIn, 0, "degrader"),
  horizontalWidth(horizontalWidthIn),
  numberWedges(numberWedgesIn),
  wedgeLength(wedgeLengthIn),
  degraderHeight(degraderHeightIn),
  degraderOffset(degraderOffsetIn),
  material(materialIn),
  colour(colourIn)
{;}

BDSDegrader::~BDSDegrader()
{;}

void BDSDegrader::BuildContainerLogicalVolume()
{
  // Input Checks
  if (horizontalWidth <= 0)
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"horizontalWidth\" is not defined or must be greater than 0" <<  G4endl;
      exit(1);
    }
  
  if (numberWedges < 1)
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"numberWedges\" is not defined or must be greater than 0" <<  G4endl;
      exit(1);
    }
  
  if (wedgeLength <= 0)
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"wedgeLength\" is not defined or must be greater than 0" <<  G4endl;
      exit(1);
    }
  
  if (degraderHeight <= 0)
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"degraderHeight\" is not defined or must be greater than 0" <<  G4endl;
      exit(1);
    }
  
  if (degraderHeight > (0.5*horizontalWidth))
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"degraderHeight\" must be less than 0.5 times \"horizontalWidth\"" <<  G4endl;
      exit(1);
    }
  
  containerSolid = new G4Box(name + "_container_solid",
			     horizontalWidth*0.5,
			     horizontalWidth*0.5,
			     chordLength*0.5);
    
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
}

void BDSDegrader::Build()
{
  BDSAcceleratorComponent::Build();
    
  G4double wedgeBasewidth = chordLength/numberWedges - lengthSafety;
    
  G4double maxzoffset = numberWedges*wedgeBasewidth * 0.5;
    
  std::vector<G4TwoVector> rightWedgeSide; // vertex co-ordinates
  std::vector<G4TwoVector> leftWedgeSide;  // vertex co-ordinates

  // Case for even number of wedges
  if (isEven(numberWedges))
    {
      for (G4int i=0; i < (numberWedges+1); i++)
	{
	  if(isEven(i))
	    {
              if(i == 0)
		{// first half wedge
                  rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                  rightWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                  rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i+0.5)*wedgeBasewidth) );
		}
              else if(i == numberWedges)
		{// last half wedge
                  rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i-0.5)*wedgeBasewidth) );
                  rightWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                  rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
		}
              else
		{// rhs full wedge(s)
                  rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i-0.5)*wedgeBasewidth) );
                  rightWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                  rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i+0.5)*wedgeBasewidth) );
		}
	    }
          else if (isOdd(i))
	    {// lhs full wedge(s)
              leftWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i-0.5)*wedgeBasewidth) );
              leftWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
              leftWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i+0.5)*wedgeBasewidth) );
	    }
	}
      
      // Vertices of base part of RHS component for connecting all RHS wedges
      rightWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength, maxzoffset) );
      rightWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength,-1.0*maxzoffset));
      
      // Vertices of base part of LHS component for connecting all LHS wedges
      leftWedgeSide.push_back( G4TwoVector(0, maxzoffset));
      leftWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength, maxzoffset) );
      leftWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength, -1.0*maxzoffset) );
      leftWedgeSide.push_back( G4TwoVector(0, -1.0*maxzoffset));
    }
  
  // Case for odd number of wedges.
  else if (isOdd(numberWedges))
    {
      for (G4int i=0; i < (numberWedges+1); i++)
	{
          if(isEven(i))
	    {
              if(i==0)
		{// rhs half wedge
                  rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                  rightWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                  rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i+0.5)*wedgeBasewidth) );
		}
              else
		{// rhs full wedge(s)
                  rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i-0.5)*wedgeBasewidth) );
                  rightWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                  rightWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i+0.5)*wedgeBasewidth) );
		}
	    }
          else if (isOdd(i))
	    {
              if(i==numberWedges)
		{// lhc half wedge
		  leftWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i-0.5)*wedgeBasewidth) );
                  leftWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                  leftWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
		}
              else
		{// lhs full wedge(s)
                  leftWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i-0.5)*wedgeBasewidth) );
                  leftWedgeSide.push_back( G4TwoVector(wedgeLength, (-1.0*maxzoffset) + (i*wedgeBasewidth)) );
                  leftWedgeSide.push_back( G4TwoVector(0, (-1.0*maxzoffset) + (i+0.5)*wedgeBasewidth) );
		}
	    }
	}

      // vertices of base part of RHS component for connecting all RHS wedges
      rightWedgeSide.push_back( G4TwoVector(0, maxzoffset));
      rightWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength, maxzoffset) );
      rightWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength,-1.0*maxzoffset));
      
      // vertices of base part of LHS component for connecting all LHS wedges
      leftWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength, maxzoffset) );
      leftWedgeSide.push_back( G4TwoVector(-0.1*wedgeLength, -1.0*maxzoffset) );
      leftWedgeSide.push_back( G4TwoVector(0, -1.0*maxzoffset));    
    }
  
  // Left wedge Solid and logical Volume
  G4ExtrudedSolid* leftWedge = new G4ExtrudedSolid(name + "_leftwedge_solid",
						   leftWedgeSide,
						   degraderHeight*0.5,
						   G4TwoVector(),1, G4TwoVector(), 1);
  
  RegisterSolid(leftWedge);
  
  G4LogicalVolume* leftWedgeLV = new G4LogicalVolume(leftWedge,                // solid
						     material,                 // material
						     name + "_leftwedge_lv");  // name
    
  // Right wedge Solid and logical Volume
  G4ExtrudedSolid* rightWedge = new G4ExtrudedSolid(name + "_rightwedge_solid",
                                                    rightWedgeSide,
                                                    degraderHeight*0.5,
                                                    G4TwoVector(),1, G4TwoVector(), 1);
      
  RegisterSolid(rightWedge);
      
  G4LogicalVolume* rightWedgeLV = new G4LogicalVolume(rightWedge,               // solid
						      material,                 // material
						      name + "_rightwedge_lv"); // name
  RegisterLogicalVolume(rightWedgeLV);
  RegisterLogicalVolume(leftWedgeLV);
  if (sensitiveOuter)
    {
      RegisterSensitiveVolume(rightWedgeLV, BDSSDType::energydep);
      RegisterSensitiveVolume(leftWedgeLV,  BDSSDType::energydep);
    }
  
  // Offsets for wedge overlap
  G4double xoffsetLeft = degraderOffset * -1.0;
  G4double xoffsetRight = degraderOffset;
    
  // Rotation  of wedges. Left taken to be +VE x direction, right is -VE x direction.
  G4RotationMatrix* rightRot = new G4RotationMatrix;  
  rightRot->rotateX(CLHEP::pi/2.0);  
  RegisterRotationMatrix(rightRot);

  G4RotationMatrix* leftRot = new G4RotationMatrix;  
  leftRot->rotateX(CLHEP::pi/-2.0);
  leftRot->rotateZ(CLHEP::pi);
  RegisterRotationMatrix(leftRot);
    
  // Wedge color
  G4VisAttributes* degraderVisAttr = new G4VisAttributes(colour);
  leftWedgeLV->SetVisAttributes(degraderVisAttr);
  rightWedgeLV->SetVisAttributes(degraderVisAttr);

  RegisterVisAttributes(degraderVisAttr);    

  // Translation of individual wedge components
  G4ThreeVector rightwedgepos(xoffsetLeft, 0, 0);
  G4ThreeVector leftwedgepos(xoffsetRight, 0, 0);
    
  // Placement of individual wedge components
  G4PVPlacement* leftwedgePV = new G4PVPlacement(leftRot,           // rotation
						 leftwedgepos,           // position
						 leftWedgeLV,            // its logical volume
						 name + "_leftwedge_pv", // its name
						 containerLogicalVolume, // its mother  volume
						 false,                  // no boolean operation
						 0,                      // copy number
						 checkOverlaps);
        
  G4PVPlacement* rightwedgePV = new G4PVPlacement(rightRot,         // rotation
						  rightwedgepos,          // position
						  rightWedgeLV,           // its logical volume
						  name + "_rightwedge_pv",// its name
						  containerLogicalVolume, // its mother  volume
						  false,                  // no boolean operation
						  0,                      // copy number
						  checkOverlaps);
    
  RegisterPhysicalVolume(leftwedgePV);
  RegisterPhysicalVolume(rightwedgePV);
}
