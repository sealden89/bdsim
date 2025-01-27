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

#ifndef BDSOCTREE_HH
#define BDSOCTREE_HH



#include "G4String.hh"
#include "G4Types.hh"
#include "G4ThreeVector.hh"
#include <vector>
/**
* @brief create octree for data
*
** @author Siobhan Alden
*
*/
class Octree {
public:
    struct Element {
        G4double x = 0;
        G4double y = 0;
        G4double z = 0;
        G4double data =0;
        G4bool isSet = false;
        Element(G4ThreeVector coords, G4double dataIn) : x(coords[0]), y(coords[1]), z(coords[2]), data(dataIn) {}
        Element(Element &point) : x(point.x), y(point.y), z(point.z), data(point.data) {}
        Element() : x(0), y(0), z(0), data(0), isSet(false) {}
        };
    Octree(G4ThreeVector lowerBounds,
             G4ThreeVector upperBounds);
    ~Octree();

    inline void setIsLeafTrue () {isLeaf = true;};
    inline void setIsLeafFalse () {isLeaf = false;};

    inline G4bool isLeafVal() {return isLeaf;};
    Element inline getDataPoint(){return dataPoint;};
    std::array<Octree*, 8> inline getChildren(){return children;};

    void insert(G4ThreeVector coords,double data);
    void createChildren();
    Octree* childToSearch(G4ThreeVector coords);
    G4bool isPointInOctant(G4ThreeVector coords);
    G4double findNearestData(G4ThreeVector incomingCoordinates);
    void setParent(Octree* parentToAssign);
    G4double distanceToData(Element data, G4ThreeVector incomingCoordinates);
    //G4double distanceToNode(Octree* node, G4ThreeVector incomingCoordinates);
    void setCorners();


    std::vector<G4double> NearestKPoints(G4int k, G4ThreeVector incomingCoordinates);

protected:
    G4double scaleFactor=1.0;
    G4bool isLeaf = false;
    Element dataPoint;
    std::array<Octree*, 8> children;

    G4ThreeVector lowerBounds;
    G4ThreeVector upperBounds;
    Octree* parent = nullptr;
    std::array<G4ThreeVector, 8> corners;

};


#endif //BDSOCTREE_HH
