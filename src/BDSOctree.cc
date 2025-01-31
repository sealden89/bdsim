

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


BDSOctree::BDSOctree(const G4ThreeVector lowerBoundsIn,
             const G4ThreeVector upperBoundsIn):
             lowerBounds(lowerBoundsIn),
             upperBounds(upperBoundsIn)
{}

BDSOctree::~BDSOctree(){
  for (auto child : children) {
    delete child; }
}

void BDSOctree::insert(G4ThreeVector point, G4double data)
{
  if (isLeaf)
  {
    if (dataPoint.isSet)
    {
      // create children by default all 8 with half bounds of existing octree
      createChildren();
      // work out which of them the point belongs in
      BDSOctree* newOctant = childToSearch(point);
      // add point with that .insert.
      newOctant->insert(point,data);
      // get old data point and find out which octant it belongs in
      BDSOctree* secondOctant = childToSearch({dataPoint.x,dataPoint.y,dataPoint.z});
      secondOctant->insert({dataPoint.x,dataPoint.y,dataPoint.z},dataPoint.data);
      setIsLeafFalse();
    }
    else
    {
      setIsLeafTrue();
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
      BDSOctree* newOctant = childToSearch(point);
      newOctant->insert(point,data);
      }
    else
      {
      // create children by default all 8 with half bounds of existing octree
      createChildren();
      // work out which of them the point belongs in
      BDSOctree* newOctant = childToSearch(point);
      // add point with that .insert.
      newOctant->insert(point,data);
      }
  }

}

void BDSOctree::setParent(BDSOctree* parentToAssign)
{
  this->parent = parentToAssign;
}

void BDSOctree::createChildren()
{
  G4double midX = (upperBounds[0] - lowerBounds[0])/2.0;
  G4double midY = (upperBounds[1] - lowerBounds[1])/2.0;
  G4double midZ = (upperBounds[2] - lowerBounds[2])/2.0;
  G4double upX = upperBounds[0];
  G4double upY = upperBounds[1];
  G4double upZ = upperBounds[2];
  G4double lowX = lowerBounds[0];
  G4double lowY = lowerBounds[1];
  G4double lowZ = lowerBounds[2];
  // filled in  backwards Z pattern top layer, top right first each layer
  children[0] = new BDSOctree({midX, midY, midZ}, upperBounds);
  children[0]->setParent(this);
  children[0]->setIsLeafTrue();
  children[1] = new BDSOctree({lowX, midY, midZ}, {midX, upY, upZ});
  children[1]->setParent(this);
  children[1]->setIsLeafTrue();
  children[2] = new BDSOctree({midX, lowY, midZ},{upX, midY, upZ});
  children[2]->setParent(this);
  children[2]->setIsLeafTrue();
  children[3] = new BDSOctree({lowX,lowY, midZ},{midX, midY, upZ});
  children[3]->setParent(this);
  children[3]->setIsLeafTrue();
  children[4] = new BDSOctree({midX, midY, lowZ},{upX, upY, midZ});
  children[4]->setParent(this);
  children[4]->setIsLeafTrue();
  children[5] = new BDSOctree({lowX, midY, lowZ},{midX, upY, midZ});
  children[5]->setParent(this);
  children[5]->setIsLeafTrue();
  children[6] = new BDSOctree({midX, lowY, lowZ}, {upX, midY, midZ});
  children[6]->setParent(this);
  children[6]->setIsLeafTrue();
  children[7] = new BDSOctree(lowerBounds, {midX, midY, midZ});
  children[7]->setParent(this);
  children[7]->setIsLeafTrue();


}
// consider binary search and then using bool logic to return the correct child instead of looping over all of them? Faster but more lines of code

BDSOctree* BDSOctree::childToSearch(G4ThreeVector coords)
{
  G4double midX = (upperBounds[0] - lowerBounds[0])/2.0;
  G4double midY = (upperBounds[1] - lowerBounds[1])/2.0;
  G4double midZ = (upperBounds[2] - lowerBounds[2])/2.0;
  G4int index = -1; // Initialize index with an invalid value
  for (size_t i = 0; i < children.size(); i++) {
    if (children[i] && children[i]->isPointInOctant(coords)) { // Check if children[i] is not null
      index = static_cast<int>(i);
      break;
    }
  }
  if (index == -1) {
    //need bds exception
    throw std::runtime_error("No child contains the point in its octant.");
  }
  // Return the child at the valid index
  return children[index];
}

G4bool BDSOctree::isPointInOctant(G4ThreeVector point)
{
  if ((point[0] <= lowerBounds[0] || point[0] >= upperBounds[0]) &&
      (point[1] <= lowerBounds[1] || point[1] >= upperBounds[1]) &&
      (point[2] <= lowerBounds[2] || point[2] >= upperBounds[2]))
  {return true;}
  else {return false;}
}

double BDSOctree::findNearestData(G4ThreeVector incomingCoordinates)
{
  if (isLeaf)
  {return dataPoint.data*scaleFactor;}
  else
    {
    BDSOctree* newOctant = childToSearch(incomingCoordinates);
    return newOctant->findNearestData(incomingCoordinates);
    }
}

G4double BDSOctree::distanceToData(Element data, G4ThreeVector incomingCoordinates)
{
  return std::sqrt((incomingCoordinates[0]-data.x)*(incomingCoordinates[0]-data.x)
                   + (incomingCoordinates[1]-data.y)*(incomingCoordinates[1]-data.y)
                   + (incomingCoordinates[2]-data.z)*(incomingCoordinates[2]-data.z));
}
void BDSOctree::setCorners()
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

/*double Octree::distanceToNode(Octree* node, std::array<double, 3> incomingCoordinates)
{
  setCorners();
  std::array<double, 8> distances;
  for (size_t i = 0; i<corners.size(); i++)
    {
    distances[i] = std::sqrt((incomingCoordinates[0]-corners[i][0])*(incomingCoordinates[0]-corners[i][0])
                             + (incomingCoordinates[1] - corners[i][1])*(incomingCoordinates[1] - corners[i][1])
                             + (incomingCoordinates[2] - corners[i][2])*(incomingCoordinates[2] - corners[i][2]));
    }

  auto minDistance = *std::min_element(distances.begin(), distances.end());
  return minDistance;
}
*/
/*
std::vector<double> NearestKPoints(int k, std::array<double, 3> incomingCoordinates)
{
  std::vector<double><k,0> closestDataPoints;
  Octree* containingOctantLeaf = childToSearch(incomingCoordinates);
  closestDataPoints[0] = containingOctantLeaf->getDataPoint();





// find the closest point.
//populate the closest list from the siblings
// go up in the octree and calculate the shortest distance to the nodes
// if the nodes are closer than the values in the shortest list then search the octants of those nodes.
// else return the nearest points.


  return 0;

}
*/