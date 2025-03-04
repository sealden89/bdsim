

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
#include "BDSOctree.hh"

#include <element.h>


BDSOctree::BDSOctree(const G4ThreeVector lowerBoundsIn,
                     const G4ThreeVector upperBoundsIn):
             lowerBounds(lowerBoundsIn),
             upperBounds(upperBoundsIn)
{
  children.fill(nullptr);


}

BDSOctree::~BDSOctree(){
  for (auto& child : children)
  {
    if (child) {
      delete child;
      child = nullptr;
    }
  }
}

void BDSOctree::Insert(G4ThreeVector point, G4double data)
{
  if (isLeaf)
  {
    if (dataPoint.isSet)
    {
      // create children by default all 8 with half bounds of existing octree
      CreateChildren();
      // work out which of them the point belongs in
      BDSOctree* newOctant = ChildToSearch(point);
      // add point with that .insert.
      newOctant->Insert(point,data);
      // get old data point and find out which octant it belongs in
      BDSOctree* secondOctant = ChildToSearch({dataPoint.x,dataPoint.y,dataPoint.z});
      secondOctant->Insert({dataPoint.x,dataPoint.y,dataPoint.z},dataPoint.data);
      SetIsLeafFalse();
    }
    else
    {
      dataPoint.x = point[0];
      dataPoint.y = point[1];
      dataPoint.z = point[2];
      dataPoint.data = data;
      dataPoint.isSet = true;
    }
  }

  else
  {
    if (children[0]!= nullptr)
      {
        // binary search which octant this belongs in
        // recurse this function on that octree
      BDSOctree* newOctant = ChildToSearch(point);
      newOctant->Insert(point,data);
      }
    else
      {
      // create children by default all 8 with half bounds of existing octree
      CreateChildren();
      // work out which of them the point belongs in
      BDSOctree* newOctant = ChildToSearch(point);
      // add point with that .insert.
      newOctant->Insert(point,data);

      }
  }

}

void BDSOctree::SetParent(BDSOctree* parentToAssign)
{
  this->parent = parentToAssign;
}

void BDSOctree::CreateChildren()
{
  G4double midX = lowerBounds[0] + (upperBounds[0] - lowerBounds[0]) / 2.0;
  G4double midY = lowerBounds[1] + (upperBounds[1] - lowerBounds[1]) / 2.0;
  G4double midZ = lowerBounds[2] + (upperBounds[2] - lowerBounds[2]) / 2.0;
  G4double upX = upperBounds[0];
  G4double upY = upperBounds[1];
  G4double upZ = upperBounds[2];
  G4double lowX = lowerBounds[0];
  G4double lowY = lowerBounds[1];
  G4double lowZ = lowerBounds[2];
  // filled in  backwards Z pattern top layer, top left first each layer
  children[0] = new BDSOctree({lowX, midY, midZ}, {midX,upY,upZ});
  children[0]->SetParent(this);
  children[0]->SetIsLeafTrue();
  children[1] = new BDSOctree({lowX,midY,lowZ}, {midX, upY, midZ});
  children[1]->SetParent(this);
  children[1]->SetIsLeafTrue();
  children[2] = new BDSOctree({midX, midY, midZ},{upX, upY, upZ});
  children[2]->SetParent(this);
  children[2]->SetIsLeafTrue();
  children[3] = new BDSOctree({midX, midY, lowZ},{upX, upY, midZ});
  children[3]->SetParent(this);
  children[3]->SetIsLeafTrue();
  children[4] = new BDSOctree({lowX,lowY,midZ},{midX, midY, upZ});
  children[4]->SetParent(this);
  children[4]->SetIsLeafTrue();
  children[5] = new BDSOctree({lowX, lowY, lowZ},{midX, midY, midZ});
  children[5]->SetParent(this);
  children[5]->SetIsLeafTrue();
  children[6] = new BDSOctree({midX, lowY,midZ}, {upX, midY, upZ});
  children[6]->SetParent(this);
  children[6]->SetIsLeafTrue();
  children[7] = new BDSOctree({midX,lowY, lowZ}, {upX, midY, midZ});
  children[7]->SetParent(this);
  children[7]->SetIsLeafTrue();


}
// consider binary search and then using bool logic to return the correct child instead of looping over all of them? Faster but more lines of code

BDSOctree* BDSOctree::ChildToSearch(G4ThreeVector coords)
{
  if (coords[0] < lowerBounds[0] || coords[0] > upperBounds[0] ||
  coords[1] < lowerBounds[1] || coords[1] > upperBounds[1] ||
  coords[2] < lowerBounds[2] || coords[2] > upperBounds[2])
  {
    if (this->parent != nullptr) {
      return this->parent;
    }
    else
    {
      // need to update to make a default octree with the data set but values of 0 in the whole thing.
      // outside of the octree the value should be 0 by default -- construct and return it then check the implementation
      // the calls to this function
      throw std::runtime_error("No child contains the point in its octant.");
    }
  }
  G4double midX = lowerBounds[0] + (upperBounds[0] - lowerBounds[0]) / 2.0;
  G4double midY = lowerBounds[1] + (upperBounds[1] - lowerBounds[1]) / 2.0;
  G4double midZ = lowerBounds[2] + (upperBounds[2] - lowerBounds[2]) / 2.0;
  G4bool xOct = (coords[0] >= midX);
  G4bool yOct = (coords[1] >= midY);
  G4bool zOct = (coords[2] >= midZ);
  G4String key= std::to_string(xOct)+std::to_string(yOct)+std::to_string(zOct);
  G4int index;
  if (key=="011"){index=0;}
  else if (key=="010"){index=1;}
  else if (key=="111"){index=2;}
  else if (key=="110"){index=3;}
  else if (key=="001"){index=4;}
  else if (key=="000"){index=5;}
  else if (key=="101"){index=6;}
  else if (key=="100"){index=7;}
  else {
  // Handle error or undefined key
  std::cerr << "Error: Unrecognized key value!" << std::endl;
  return nullptr; // or some appropriate default/error value
  }

  return children[index];
}

double BDSOctree::FindNearestData(G4ThreeVector incomingCoordinates)
{
  if (isLeaf)
  {
    if (dataPoint.isSet)
    {
      return dataPoint.data * scaleFactor;
    }
    else
    {
      std::array<BDSOctree*, 8> siblings = this->parent->GetChildren();
      std::vector<std::pair<G4double, G4double>> distanceDataPairs;

      for (BDSOctree* sibling : siblings)
      {
        if (sibling->dataPoint.isSet)
        {
          G4ThreeVector data(sibling->dataPoint.x, sibling->dataPoint.y, sibling->dataPoint.z);
          G4double dist = (incomingCoordinates - data).mag();
          distanceDataPairs.emplace_back(dist, sibling->dataPoint.data);
        }
      }

      if (distanceDataPairs.empty())
      {
        return 0.0; // Handle case where no sibling has data set
      }

      auto nearest = std::min_element(distanceDataPairs.begin(), distanceDataPairs.end(),
                                      [](const auto& a, const auto& b) { return a.first < b.first; });

      return nearest->second * scaleFactor;
    }
  }
  else
  {
    BDSOctree* newOctant = ChildToSearch(incomingCoordinates);
    return newOctant->FindNearestData(incomingCoordinates);
  }
}

G4double BDSOctree::DistanceToData(Element data, G4ThreeVector incomingCoordinates)
{
  return std::sqrt((incomingCoordinates[0]-data.x)*(incomingCoordinates[0]-data.x)
                   + (incomingCoordinates[1]-data.y)*(incomingCoordinates[1]-data.y)
                   + (incomingCoordinates[2]-data.z)*(incomingCoordinates[2]-data.z));
}

void BDSOctree::SetCorners()
{
  corners[0] = upperBounds;
  corners[1] = {lowerBounds[0], upperBounds[1], upperBounds[2]};
  corners[2] = {upperBounds[0], lowerBounds[1], upperBounds[2]};
  corners[3] = {lowerBounds[0], lowerBounds[1], upperBounds[2]};
  corners[4] = {upperBounds[0], upperBounds[1], lowerBounds[2]};
  corners[5] = {lowerBounds[0], upperBounds[1], lowerBounds[2]};
  corners[6] = {upperBounds[0], lowerBounds[1], lowerBounds[2]};
  corners[7] = lowerBounds;
}

double BDSOctree::DistanceToNode(BDSOctree* node, G4ThreeVector incomingCoordinates)
{
  SetCorners();
  std::array<double, 8> distances;
  for (size_t i = 0; i<corners.size(); i++)
    {
    distances[i] = std::sqrt((incomingCoordinates[0]-corners[i][0])*(incomingCoordinates[0]-corners[i][0])
                             + (incomingCoordinates[1] - corners[i][1])*(incomingCoordinates[1] - corners[i][1])
                             + (incomingCoordinates[2] - corners[i][2])*(incomingCoordinates[2] - corners[i][2]));
    }

  auto maxDistance = *std::max_element(distances.begin(), distances.end());
  return maxDistance;
}
