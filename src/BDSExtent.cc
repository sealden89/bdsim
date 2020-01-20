/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSExtent.hh"
#include "BDSParticleCoords.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

#include <algorithm>
#include <ostream>
#include <utility>
#include <vector>

BDSExtent::BDSExtent():
  extXNeg(0.0),
  extXPos(0.0),
  extYNeg(0.0),
  extYPos(0.0),
  extZNeg(0.0),
  extZPos(0.0)
{;}

BDSExtent::BDSExtent(const std::pair<G4double, G4double>& extXIn,
		     const std::pair<G4double, G4double>& extYIn,
		     const std::pair<G4double, G4double>& extZIn):
  extXNeg(extXIn.first),
  extXPos(extXIn.second),
  extYNeg(extYIn.first),
  extYPos(extYIn.second),
  extZNeg(extZIn.first),
  extZPos(extZIn.second)
{;}

BDSExtent::BDSExtent(G4double extXNegIn, G4double extXPosIn,
		     G4double extYNegIn, G4double extYPosIn,
		     G4double extZNegIn, G4double extZPosIn):
  extXNeg(extXNegIn),
  extXPos(extXPosIn),
  extYNeg(extYNegIn),
  extYPos(extYPosIn),
  extZNeg(extZNegIn),
  extZPos(extZPosIn)
{;}

BDSExtent::BDSExtent(G4double extXIn, G4double extYIn, G4double extZIn):
  extXNeg(-std::abs(extXIn)),
  extXPos( std::abs(extXIn)),
  extYNeg(-std::abs(extYIn)),
  extYPos( std::abs(extYIn)),
  extZNeg(-std::abs(extZIn)),
  extZPos( std::abs(extZIn))
{;}

BDSExtent::BDSExtent(G4ThreeVector extIn):
  BDSExtent(extIn.x(), extIn.y(), extIn.z())
{;}

BDSExtent::BDSExtent(G4ThreeVector extInNeg,
                     G4ThreeVector extInPos):
  extXNeg(extInNeg.x()),
  extXPos(extInPos.x()),
  extYNeg(extInNeg.y()),
  extYPos(extInPos.y()),
  extZNeg(extInNeg.z()),
  extZPos(extInPos.z())
{;}

BDSExtent::~BDSExtent()
{;}

std::vector<G4ThreeVector> BDSExtent::AllBoundaryPoints() const
{
  std::vector<G4ThreeVector> result;
  result.emplace_back(extXNeg, extYNeg, extZNeg);
  result.emplace_back(extXNeg, extYPos, extZNeg);
  result.emplace_back(extXPos, extYNeg, extZNeg);
  result.emplace_back(extXPos, extYPos, extZNeg);
  result.emplace_back(extXNeg, extYNeg, extZPos);
  result.emplace_back(extXNeg, extYPos, extZPos);
  result.emplace_back(extXPos, extYNeg, extZPos);
  result.emplace_back(extXPos, extYPos, extZPos);
  return result;
}

BDSExtent BDSExtent::TiltOffset(const BDSTiltOffset* tiltOffset) const
{
  if (!tiltOffset)
    {return BDSExtent(*this);}
  BDSExtent tilted = Tilted(tiltOffset->GetTilt());
  BDSExtent offset = tilted.Translate(tiltOffset->GetOffset());
  return offset;
}

BDSExtent BDSExtent::Translate(G4double dx, G4double dy, G4double dz) const
{
  return BDSExtent(extXNeg + dx, extXPos + dx,
		   extYNeg + dy, extYPos + dy,
		   extZNeg + dz, extZPos + dz);
}

BDSExtent BDSExtent::Tilted(G4double angle) const
{
  if (!BDS::IsFinite(angle))
    {return BDSExtent(*this);}

  // rotate each vector (from origin to each corner) by angle
  // and check - safer than checking based on +ve / -ve angle
  G4TwoVector topRight = G4TwoVector(extXPos, extYPos);
  G4TwoVector botRight = G4TwoVector(extXPos, extYNeg);
  G4TwoVector botLeft  = G4TwoVector(extXNeg, extYNeg);
  G4TwoVector topLeft  = G4TwoVector(extXNeg, extYPos);

  topRight.rotate(angle);
  botRight.rotate(angle);
  botLeft.rotate(angle);
  topLeft.rotate(angle);
  
  G4double xMin = std::min(botLeft.x(),  topLeft.x());
  G4double xMax = std::max(topRight.x(), botRight.x());
  G4double yMin = std::min(botRight.y(), botLeft.y());
  G4double yMax = std::max(topRight.y(), topLeft.y());

  BDSExtent result = BDSExtent(xMin, xMax,
			       yMin, yMax,
			       extZNeg, extZPos);
  return result;
}

std::ostream& operator<< (std::ostream& out, BDSExtent const& ext)
{
  out << ext.extXNeg << " " << ext.extXPos << " ";
  out << ext.extYNeg << " " << ext.extYPos << " ";
  out << ext.extZNeg << " " << ext.extZPos;
  return out;
}

G4double BDSExtent::MaximumAbs() const
{
  return std::max({std::abs(extXNeg), extXPos,
		   std::abs(extYNeg), extYPos,
		   std::abs(extZNeg), extZPos});
}

G4double BDSExtent::MinimumAbs() const
{
  return std::min({std::abs(extXNeg), extXPos,
		   std::abs(extYNeg), extYPos,
		   std::abs(extZNeg), extZPos});
}

G4double BDSExtent::MaximumAbsTransverse() const
{
  return std::max({std::abs(extXNeg), extXPos,
		   std::abs(extYNeg), extYPos});
}

G4double BDSExtent::MinimumAbsTransverse() const
{
  return std::min({std::abs(extXNeg), extXPos,
		   std::abs(extYNeg), extYPos});
}

G4bool BDSExtent::Encompasses(const G4ThreeVector& point) const
{
  BDSExtent extentPoint = BDSExtent(point);
  return extentPoint < (*this);
}

G4bool BDSExtent::Encompasses(const BDSExtent& other) const
{
  std::vector<G4ThreeVector> otherPoints = other.AllBoundaryPoints();
  G4bool result = false;
  for (const auto& p : otherPoints)
    {result = result || !Encompasses(p);}
  return !result;
}

G4bool BDSExtent::Encompasses(const BDSParticleCoords& coords) const
{
  G4ThreeVector point(coords.x, coords.y, coords.z);
  return Encompasses(point);
}

BDSExtent BDSExtent::ExpandBy(G4double margin) const
{
  margin = std::abs(margin);
  BDSExtent result(*this);
  result.extXNeg -= margin;
  result.extXPos += margin;
  result.extYNeg -= margin;
  result.extYPos += margin;
  result.extZNeg -= margin;
  result.extZPos += margin;
  return result;
}

BDSExtent BDSExtent::ExpandTransverselyBy(G4double margin) const
{
  BDSExtent result(*this);
  result.extXNeg -= margin;
  result.extXPos += margin;
  result.extYNeg -= margin;
  result.extYPos += margin;
  return result;
}
