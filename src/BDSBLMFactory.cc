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
#include "BDSBLM.hh"
#include "BDSBLMFactory.hh"
#include "BDSBLMType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Orb.hh"
#include "G4Tubs.hh"

BDSBLMFactory::BDSBLMFactory()
{;}

BDSBLMFactory::~BDSBLMFactory()
{;}

BDSBLM* BDSBLMFactory::BuildBLM(G4String name,
				G4String geometryFile,
				G4String geometryType,
				G4String material,
				G4double blm1,
				G4double blm2,
				G4double blm3,
				G4double /*blm4*/)
{
  // if geometry file is specified then we load the external file.
  BDSBLM* result = nullptr;
  if (!geometryFile.empty())
    {
      BDSGeometryExternal* blmGeom = BDSGeometryFactory::Instance()->BuildGeometry(name,
										   geometryFile);

      result = new BDSBLM(blmGeom);
    }
  else
    {
      BDSBLMType shape = BDS::DetermineBLMType(geometryType);
      switch (shape.underlying())
	{
	case BDSBLMType::cylinder:
	  {result = BuildBLMCylinder(name, material, blm1, blm2); break;}
	case BDSBLMType::cube:
	  {result = BuildBLMCube(name, material, blm1, blm2, blm3); break;}
	case BDSBLMType::sphere:
	  {result = BuildBLMSphere(name, material, blm1); break;}
	default:
	  {break;}
	}
    }
  if (!result)
    {return result;}

  // sensitivity
  // register with output

  return result;
}

BDSBLM* BDSBLMFactory::BuildBLMCylinder(G4String name,
					G4String material,
					G4double halfLength,
					G4double radius)
{
  if (!BDS::IsFinite(halfLength) || halfLength < 0)
    {throw BDSException(__METHOD_NAME__, "\"blm1\" (half length) must be +ve finite for blm named \"" + name + "\"");}
  if (!BDS::IsFinite(radius) || radius < 0)
    {throw BDSException(__METHOD_NAME__, "\"blm2\" (radius) must be +ve finite for blm named \"" + name + "\"");}
  G4Tubs* shape = new G4Tubs(name + "_solid",
			     0,
			     radius,
			     halfLength,
			     0,
			     CLHEP::twopi);
  BDSExtent ext = BDSExtent(radius, radius, halfLength);
  return CommonConstruction(name, material, shape, ext);
}

BDSBLM* BDSBLMFactory::BuildBLMCube(G4String name,
				    G4String material,
				    G4double halfLengthX,
				    G4double halfLengthY,
				    G4double halfLengthZ)
{
  if (!BDS::IsFinite(halfLengthX) || halfLengthX < 0)
    {throw BDSException(__METHOD_NAME__, "\"blm1\" (half length in x) must be +ve finite for blm named \"" + name + "\"");}
  if (!BDS::IsFinite(halfLengthY) || halfLengthY < 0)
    {throw BDSException(__METHOD_NAME__, "\"blm2\" (half length in y) must be +ve finite for blm named \"" + name + "\"");}
  if (!BDS::IsFinite(halfLengthZ) || halfLengthZ < 0)
    {throw BDSException(__METHOD_NAME__, "\"blm3\" (half length in z) must be +ve finite for blm named \"" + name + "\"");}
  G4Box* shape = new G4Box(name + "_solid", halfLengthX, halfLengthY, halfLengthZ);
  BDSExtent ext = BDSExtent(halfLengthX, halfLengthY, halfLengthZ);
  return CommonConstruction(name, material, shape, ext);
}

BDSBLM* BDSBLMFactory::BuildBLMSphere(G4String name,
				      G4String material,
				      G4double radius)
{
  if (!BDS::IsFinite(radius) || radius < 0)
    {throw BDSException(__METHOD_NAME__, "\"blm1\" (radius) must be +ve finite for blm named \"" + name + "\"");}
  G4Orb* shape = new G4Orb(name + "_solid", radius);
  BDSExtent ext = BDSExtent(radius, radius, radius);
  return CommonConstruction(name, material, shape, ext);
}

BDSBLM* BDSBLMFactory::CommonConstruction(G4String  name,
					  G4String  material,
					  G4VSolid* shape,
					  BDSExtent extent)
{
  G4Material* mat = BDSMaterials::Instance()->GetMaterial(material);
  G4LogicalVolume* lv = new G4LogicalVolume(shape, mat, name + "_lv");
  
  BDSBLM* result = new BDSBLM(shape, lv, extent);
  return result;
}
