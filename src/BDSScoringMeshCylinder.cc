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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSHistBinMapper.hh"
#include "BDSScoringMeshCylinder.hh"
#include "BDSScorerMeshInfo.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4Version.hh"

BDSScoringMeshCylinder::BDSScoringMeshCylinder(const G4String&          name,
                                     const BDSScorerMeshInfo& recipe,
                                     const G4Transform3D&     placementTransform):
    BDSScoringMeshCylinder(name, placementTransform.getTranslation(), placementTransform.getRotation())
{
  // size of the scoring mesh
#if G4VERSION_NUMBER < 1100
  scorerSize[0] = recipe.ScoringMeshR(); // RMax
  scorerSize[1] = recipe.ScoringMeshZ(); // ZSize
#else
  scorerSize[0] = 0;  // RMin
  scorerSize[1] = recipe.ScoringMeshR(); // RMax
  scorerSize[2] = recipe.ScoringMeshZ(); // ZSize
#endif
  SetSize(scorerSize);

  // divisions of the scoring mesh
  nSegment[0] = recipe.nBinsZ;
  nSegment[1] = recipe.nBinsPhi;
  nSegment[2] = recipe.nBinsR;
  nEnergySegments = recipe.nBinsE;
  SetNumberOfSegments(nSegment);

#ifdef USE_BOOST
  mapper = new BDSHistBinMapper(fNSegment[0], fNSegment[1], fNSegment[2], nEnergySegments, recipe.energyAxis);
#else
  mapper = new BDSHistBinMapper(fNSegment[0], fNSegment[1], fNSegment[2], nEnergySegments);
#endif
}


BDSScoringMeshCylinder::BDSScoringMeshCylinder(const G4String&         name,
                                     const G4ThreeVector&    translation,
                                     const G4RotationMatrix& rotation):
  G4ScoringCylinder(name),
  mapper(nullptr)
{
  fRotationMatrix = new G4RotationMatrix(rotation.inverse());
  fCenterPosition = translation;
}

const BDSHistBinMapper* BDSScoringMeshCylinder::Mapper() const
{
  if (!sizeIsSet)
    {throw BDSException(__METHOD_NAME__, "mesh \"" + fWorldName + "\" size not set but queried");}
  if (!mapper)
    {throw BDSException(__METHOD_NAME__, "mesh \"" + fWorldName + "\" mapper has mysteriously disappeared");}
  return mapper;
}

BDSScoringMeshCylinder::~BDSScoringMeshCylinder()
{
  delete mapper;
}
