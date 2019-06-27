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


#include "BDSElectronOccupancy.hh"
#include "BDSUtilities.hh"


BDSElectronOccupancy::BDSElectronOccupancy(G4int sizeOrbit)
{
    Populate2DOccupancy();

    theSizeOfOrbit =0;

    // allocate and clear the array of theOccupancies
    theOccupancies = new G4int[theSizeOfOrbit];
    G4int   index =0;
    for (index = 0; index <  theSizeOfOrbit; index++) {
        theOccupancies[index] = 0;
    }
    SetTheTotalOccupancy(0);
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

    Populate2DOccupancy();

}

BDSElectronOccupancy::~BDSElectronOccupancy()
{}

void BDSElectronOccupancy::Populate2DOccupancy()
{

    PopulateOccupancyOrder();
    G4int size = occupancyOrder.size();
    G4int totalOccupancy = theTotalOccupancy;

    for(G4int index = 0; index<size;index++)
    {
        G4int n = std::get<0>(occupancyOrder[index]);
        G4int l = std::get<1>(occupancyOrder[index]);
        G4int max = std::get<2>(occupancyOrder[index]);
        if(max>14)
        {
            theOccupancies2D[n][l]=max;
        }
        else
        {
            if(totalOccupancy>0)
            {
                if(theOccupancies2D[n][l]<max)
                {
                    if((totalOccupancy-max)>=0)
                    {
                        theOccupancies2D[n][l]=max;
                        totalOccupancy=totalOccupancy-max;
                    }
                    else
                    {
                        theOccupancies2D[n][l]=totalOccupancy;
                        totalOccupancy=0;
                    }
                }
            }
            else
            {
                theOccupancies2D[n][l]=0;
            }

        }
    }

}

G4int BDSElectronOccupancy::AddElectron(G4int orbit, G4int shape, G4int number)
{
    G4int value=0;
    G4int theMaxOccupancy=0;
    if(shape==0){theMaxOccupancy=sMaxOccupancy;}
    else if(shape==1){theMaxOccupancy=pMaxOccupancy;}
    else if(shape==2){theMaxOccupancy==dMaxOccupancy;}
    else if(shape==3){theMaxOccupancy==fMaxOccupancy;}
    if(theOccupancies2D[orbit][shape]<theMaxOccupancy)
    {
        theOccupancies2D[orbit][shape] += number;
    }
    else {//error? or change the l or n number?}
    }
    return value;
}



G4int BDSElectronOccupancy::RemoveElectron(G4int orbit, G4int shape, G4int number)
{
    G4int value=0;
    if(theOccupancies2D[orbit][shape]<0)
    {
        theOccupancies2D[orbit][shape] -= number;
        value = number;

    }
    else {//error? or change the l or n number?}
    }
    return value;

}


void BDSElectronOccupancy::PopulateOccupancyOrder()
{
    occupancyOrder.push_back({0,0,notAllowed});
    occupancyOrder.push_back({0,1,notAllowed});
    occupancyOrder.push_back({0,2,notAllowed});
    occupancyOrder.push_back({0,3,notAllowed});
    occupancyOrder.push_back({1,0,sMaxOccupancy});
    occupancyOrder.push_back({1,1,notAllowed});
    occupancyOrder.push_back({2,0,sMaxOccupancy});
    occupancyOrder.push_back({1,2,notAllowed});
    occupancyOrder.push_back({2,1,pMaxOccupancy});
    occupancyOrder.push_back({3,0,sMaxOccupancy});
    occupancyOrder.push_back({1,3,notAllowed});
    occupancyOrder.push_back({2,2,notAllowed});
    occupancyOrder.push_back({3,1,pMaxOccupancy});
    occupancyOrder.push_back({4,0,sMaxOccupancy});
    occupancyOrder.push_back({2,3,notAllowed});
    occupancyOrder.push_back({3,2,dMaxOccupancy});
    occupancyOrder.push_back({4,1,pMaxOccupancy});
    occupancyOrder.push_back({5,0,sMaxOccupancy});
    occupancyOrder.push_back({3,3,notAllowed});
    occupancyOrder.push_back({4,2,dMaxOccupancy});
    occupancyOrder.push_back({5,1,pMaxOccupancy});
    occupancyOrder.push_back({6,0,sMaxOccupancy});
    occupancyOrder.push_back({4,3,fMaxOccupancy});
    occupancyOrder.push_back({5,2,dMaxOccupancy});
    occupancyOrder.push_back({6,1,pMaxOccupancy});
    occupancyOrder.push_back({7,0,sMaxOccupancy});
    occupancyOrder.push_back({5,3,fMaxOccupancy});
    occupancyOrder.push_back({6,2,dMaxOccupancy});
    occupancyOrder.push_back({7,1,pMaxOccupancy});
    occupancyOrder.push_back({6,3,notAllowed});
    occupancyOrder.push_back({7,2,notAllowed});
    occupancyOrder.push_back({7,3,notAllowed});


}