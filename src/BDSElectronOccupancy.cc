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

/*
#include "BDSElectronOccupancy.hh"



BDSElectronOccupancy::BDSElectronOccupancy(G4int sizeOrbit):
G4ElectronOccupancy(sizeOrbit),
theShapeOfOrbit(sizeOrbit-1)
{
    theOccupancies = new G4int[theSizeOfOrbit][theShapeOfOrbit];

    //theSizeOfOrbit = max principle quantum number, n.
    // theShapeOfOrbit = angular quantum number, l.
    for(G4int nIndex = 1;nIndex = theSizeOfOrbit; nIndex++)
    {
     for(G4int lIndex = 0; lIndex = theShapeOfOrbit ;lIndex++)
     {
     theOccupancies[nIndex][lIndex]=0;
     }
    }

    theTotalOccupancy=0;
}

BDSElectronOccupancy::BDSElectronOccupancy(const G4ElectronOccupancy &right)
{
    theSizeOfOrbit = right.GetSizeOfOrbit();

    // allocate and clear the array of theOccupancies
    theOccupancies = new G4int[theSizeOfOrbit];
    G4int   index =0;
    for (index = 0; index <  theSizeOfOrbit; index++) {
        theOccupancies[index] = right.GetOccupancy(index);
    }

    SetTheTotalOccupancy(right.GetTotalOccupancy());

}


G4int BDSElectronOccupancy::AddElectron(G4int orbit, G4int shape, G4int number)
{
    G4int value=0;
    G4int maxOccupancy;
    if(shape == 0)
    {
        maxOccupancy=sMaxOccupancy;
        if(theOccupancies[orbit][shape]<maxOccupancy)
        {
            theOccupancies[orbit][shape] +=1;
            theTotalOccupancy +=1;
            value=number;
        }
        else
        {
            shape +=1;
        }
    }
    else if(shape == 1)
    {
        maxOccupancy=pMaxOccupancy;
        if(theOccupancies[orbit][shape]<maxOccupancy)
        {
            theOccupancies[orbit][shape] +=1;
            theTotalOccupancy +=1;
            value=number;
        }
        else
        {
            orbit +=1;
            theOccupancies[orbit][shape] +=1;
            theTotalOccupancy +=1;
            value=number;
        }

    }
    return value;

}





G4int BDSElectronOccupancy::RemoveElectron(G4int orbit, G4int shape, G4int number)
{
    G4int value=0;
    G4int maxOccupancy;
    if (theOccupancies[orbit][shape] >= number)
    {
     theOccupancies[orbit][shape] -= number;
        value = number;
    }
    else if (theOccupancies[orbit][shape] < number)
    {
        theOccupancies[orbit][shape] = 0;
        if (shape > 0 & shape <=orbit-1)
        {
            shape-=1;
            if (theOccupancies[orbit][shape] >= number)
            {
                theOccupancies[orbit][shape] -= number;
                value = number;
            // ok this is clearly a recursive function waiting to happen!
            }

        }
    }
    return value;

}
*/