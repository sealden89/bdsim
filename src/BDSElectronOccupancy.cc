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


BDSElectronOccupancy::BDSElectronOccupancy(G4int maxn)
{
    G4double spinUp=0.5;
    G4double spinDown=-0.5;
    for(G4int i=1; i<=maxn; i++)
    {
        for(G4int j=0; j<i; j++)
        {
            G4double jSpinOrbit1 = abs(j+spinUp);
            G4double jSpinOrbit2 = abs(j+spinDown);
            if(jSpinOrbit1==jSpinOrbit2)
            {
                stateList.push_back(new BDSElectronQuantumLevel(i,j,jSpinOrbit1));
            }
            else
            {
                stateList.push_back(new BDSElectronQuantumLevel(i,j,jSpinOrbit2));
                stateList.push_back(new BDSElectronQuantumLevel(i,j,jSpinOrbit1));
            }
        }
    }

    sort(stateList.begin(),stateList.end(),CompareEnergy);

}

BDSElectronOccupancy::~BDSElectronOccupancy()
{}

void BDSElectronOccupancy::CreateNewLevel(G4int n, G4int l, G4int j)
{
    stateList.push_back(new BDSElectronQuantumLevel(n,l,j));
}

G4bool BDSElectronOccupancy::CompareEnergy(BDSElectronQuantumLevel* level1, BDSElectronQuantumLevel* level2)
{
    return (level1->GetLevelEnergy()>level2->GetLevelEnergy());
}

void BDSElectronOccupancy::PopulateLevels()
{
    G4int size=stateList.size();
    G4int currentElectrons = totalElectrons;
    for(G4int i=0; i<size; i++) {
        if (currentElectrons > 0) {
            G4int max = stateList[i]->GetMaxOccupancy();
            G4int current = stateList[i]->GetCurrentOccupancy();
            if (max > current) {
                if ((max - current) < currentElectrons) {
                    stateList[i]->AddElectrons(max - current);
                    currentElectrons = currentElectrons - (max - current);
                }
                else
                {
                    stateList[i]->AddElectrons(currentElectrons);
                    currentElectrons = 0;
                }
            }
        }
    }

}

void BDSElectronOccupancy::AddElectrons(G4int number)
{
    G4int size=stateList.size();
    G4int currentElectrons = number;
    for(G4int i=0; i<size; i++) {
        if (currentElectrons > 0) {
            G4int max = stateList[i]->GetMaxOccupancy();
            G4int current = stateList[i]->GetCurrentOccupancy();
            if (max > current) {
                if ((max - current) < currentElectrons) {
                    stateList[i]->AddElectrons(max - current);
                    currentElectrons = currentElectrons - (max - current);
                }
                else
                {
                    stateList[i]->AddElectrons(currentElectrons);
                    currentElectrons = 0;
                }
            }
        }
    }

}

void BDSElectronOccupancy::AddElectrons(G4int n, G4int l, G4int number)
{

    G4int size=stateList.size();
    G4int currentNumber=number;
    for(G4int i=0; i<size;i++)
    {
        if(stateList[i]->GetnPrincipleNumnber()==n&&stateList[i]->GetlAngularNumber()==l)
        {
            if(currentNumber>0)
            {
                G4int currentAvailable = stateList[i]->GetMaxOccupancy()-stateList[i]->GetCurrentOccupancy();
                if (currentAvailable > currentNumber) {
                    stateList[i]->AddElectrons(currentNumber);
                    currentNumber=0;
                }
               //else error
            }
        }
        else
        {
            G4double spinUp=0.5;
            G4double spinDown=-0.5;
            G4double j1 = abs(l+spinUp);
            G4double j2 = abs(l+spinDown);
            if(j1==j2)
            {
                stateList.push_back(new BDSElectronQuantumLevel(n, l, j2));
                if(stateList[stateList.size()-1]->GetMaxOccupancy()>currentNumber)
                {
                    stateList[stateList.size()-1]->AddElectrons(currentNumber);
                }
                //else error
            }
            else
            {
                stateList.push_back(new BDSElectronQuantumLevel(n, l, j2));
                if(stateList[stateList.size()-1]->GetMaxOccupancy()>currentNumber)
                {
                    stateList[stateList.size()-1]->AddElectrons(currentNumber);
                }
                //else error
                stateList.push_back(new BDSElectronQuantumLevel(n, l, j1));


            }
        }
    }
}

void BDSElectronOccupancy::RemoveElectrons(G4int n, G4int l, G4int number)
{
    G4int currentNumber=number;
    G4int size=stateList.size();
    for(G4int i=0; i<size; i++)
    {
        BDSElectronQuantumLevel* holder = stateList[i];
        if(stateList[i]->GetnPrincipleNumnber() == n && stateList[i]->GetlAngularNumber() == l)
        {
            if(currentNumber>0) {
                if (stateList[i]->GetCurrentOccupancy() > 0)
                {
                    stateList[i]->RemoveElectrons(number);
                    currentNumber=currentNumber-number;
                }
            }
        }
    }
}

G4bool BDSElectronOccupancy::StatePopulated(G4int n, G4int l)
{
    G4int size=stateList.size();
    for(int i=0; i<size;i++)
    {
        BDSElectronQuantumLevel* holder2 = stateList[i];

        if (stateList[i]->GetnPrincipleNumnber() == n && stateList[i]->GetlAngularNumber() == l)
        {
            BDSElectronQuantumLevel* holder = stateList[i];

            if(stateList[i]->GetCurrentOccupancy()>0){ return true;}

        }
    }
    return false;
}

void BDSElectronOccupancy::SetStateLifetime(G4int n, G4int l, G4double lifetime)
{
    G4int size=stateList.size();
    for(int i=0; i<size; i++)
    {
        if (stateList[i]->GetnPrincipleNumnber() == n && stateList[i]->GetlAngularNumber() == l)
        {
            if(stateList[i]->GetCurrentOccupancy()>0)
            {
                stateList[i]->SetExcitedLifetime(lifetime);
            }
        }
    }
}

BDSElectronQuantumLevel* BDSElectronOccupancy::GetQuantumLevel(G4int n, G4int l, G4double j)
{
    G4int size=stateList.size();
    for(int i=0; i<size; i++)
    {
        if (stateList[i]->GetnPrincipleNumnber() == n && stateList[i]->GetlAngularNumber() == l && stateList[i]->GetjSpinOrbitCoupling()==j)
        {
           return stateList[i];
        }
    }

}

void BDSElectronOccupancy::SetTimeOfExciation(G4double timeOfExcitationIn, G4int n, G4int l, G4double j)
{
    G4int size=stateList.size();
    for(int i=0; i<size; i++)
    {
        if (stateList[i]->GetnPrincipleNumnber() == n && stateList[i]->GetlAngularNumber() == l && stateList[i]->GetjSpinOrbitCoupling()==j)
        {
            stateList[i]->SetTimeOfExcitement(timeOfExcitationIn);
        }
    }
}

G4double BDSElectronOccupancy::GetStateLifetime(G4int n, G4int l, G4double j)
{
    G4int size=stateList.size();
    for(int i=0; i<size; i++)
    {
        if (stateList[i]->GetnPrincipleNumnber() == n && stateList[i]->GetlAngularNumber() == l && stateList[i]->GetjSpinOrbitCoupling()==j)
        {
            return stateList[i]->GetExcitedLifetime();
        }
    }
}